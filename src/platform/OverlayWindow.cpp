#include "platform/OverlayWindow.h"
#include <cstring>

namespace novamap {
long long OverlayWindow::sClassAtom = 0;

#ifdef _WIN32
namespace {
LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_NCHITTEST) return HTTRANSPARENT;
    if (msg == WM_ERASEBKGND) return 1;
    if (msg == WM_DESTROY) return 0;
    return DefWindowProcW(hwnd, msg, wp, lp);
}

struct FindWindowData { DWORD pid; HWND hwnd; };
BOOL CALLBACK enumWindowsProc(HWND hwnd, LPARAM lparam) {
    auto* data = reinterpret_cast<FindWindowData*>(lparam);
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != data->pid || !IsWindowVisible(hwnd) || GetWindow(hwnd, GW_OWNER) != nullptr) return TRUE;
    wchar_t title[128]{};
    GetWindowTextW(hwnd, title, 128);
    if (title[0] == L'\0') return TRUE;
    data->hwnd = hwnd;
    return FALSE;
}
}
#endif

OverlayWindow::~OverlayWindow() { destroy(); }

bool OverlayWindow::create(int mapSize, int scale, int margin) {
#ifdef _WIN32
    mMapSize = mapSize; mScale = scale; mMargin = margin;
    HINSTANCE instance = GetModuleHandleW(nullptr);
    if (!sClassAtom) {
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = wndProc;
        wc.hInstance = instance;
        wc.lpszClassName = L"NovaMapOverlayClass";
        sClassAtom = RegisterClassExW(&wc);
        if (!sClassAtom && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) return false;
    }

    int w = mMapSize * mScale;
    int h = mMapSize * mScale;
    mHwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        L"NovaMapOverlayClass", L"Nova Map", WS_POPUP,
        0, 0, w, h, nullptr, nullptr, instance, nullptr
    );
    if (!mHwnd) return false;

    HDC screen = GetDC(nullptr);
    mMemDc = CreateCompatibleDC(screen);
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = mMapSize;
    bi.bmiHeader.biHeight = -mMapSize;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    mBitmap = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &mBits, nullptr, 0);
    ReleaseDC(nullptr, screen);
    if (!mBitmap || !mBits) return false;
    mOldBitmap = SelectObject(mMemDc, mBitmap);
    setVisible(false);
    return true;
#else
    (void)mapSize; (void)scale; (void)margin; return false;
#endif
}

void OverlayWindow::destroy() {
#ifdef _WIN32
    if (mMemDc && mOldBitmap) { SelectObject(mMemDc, mOldBitmap); mOldBitmap = nullptr; }
    if (mBitmap) { DeleteObject(mBitmap); mBitmap = nullptr; }
    if (mMemDc) { DeleteDC(mMemDc); mMemDc = nullptr; }
    if (mHwnd) { DestroyWindow(mHwnd); mHwnd = nullptr; }
    mBits = nullptr;
    mVisible = false;
#endif
}

#ifdef _WIN32
HWND OverlayWindow::findGameWindow() const {
    FindWindowData data{GetCurrentProcessId(), nullptr};
    EnumWindows(enumWindowsProc, reinterpret_cast<LPARAM>(&data));
    return data.hwnd;
}
#endif

void OverlayWindow::setVisible(bool visible) {
#ifdef _WIN32
    if (!mHwnd || visible == mVisible) return;
    ShowWindow(mHwnd, visible ? SW_SHOWNOACTIVATE : SW_HIDE);
    mVisible = visible;
#else
    (void)visible;
#endif
}

void OverlayWindow::present(std::vector<std::uint32_t> const& pixels) {
#ifdef _WIN32
    if (!mHwnd || !mBits || static_cast<int>(pixels.size()) != mMapSize * mMapSize) return;
    HWND mc = findGameWindow();
    if (!mc || IsIconic(mc) || GetForegroundWindow() != mc) {
        setVisible(false);
        return;
    }

    std::memcpy(mBits, pixels.data(), pixels.size() * sizeof(std::uint32_t));

    RECT client{};
    GetClientRect(mc, &client);
    POINT topLeft{client.left, client.top};
    ClientToScreen(mc, &topLeft);

    int clientW = client.right - client.left;
    int outW = mMapSize * mScale;
    int outH = mMapSize * mScale;
    POINT dst{topLeft.x + clientW - outW - mMargin, topLeft.y + mMargin};
    SIZE size{outW, outH};
    POINT src{0,0};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

    HDC screen = GetDC(nullptr);
    UpdateLayeredWindow(mHwnd, screen, &dst, &size, mMemDc, &src, 0, &blend, ULW_ALPHA);
    ReleaseDC(nullptr, screen);
    setVisible(true);
#endif
}

void OverlayWindow::pump() {
#ifdef _WIN32
    if (!mHwnd) return;
    MSG msg{};
    while (PeekMessageW(&msg, mHwnd, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
#endif
}

} // namespace novamap
