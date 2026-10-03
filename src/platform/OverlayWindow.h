#pragma once
#ifdef _WIN32
#include <Windows.h>
#endif
#include <cstdint>
#include <vector>

namespace novamap {
class OverlayWindow {
public:
    OverlayWindow() = default;
    ~OverlayWindow();
    bool create(int mapSize, int scale = 1, int margin = 18);
    void destroy();
    void present(std::vector<std::uint32_t> const& pixels);
    void setVisible(bool visible);
    void pump();
private:
#ifdef _WIN32
    HWND mHwnd{nullptr};
    HBITMAP mBitmap{nullptr};
    HGDIOBJ mOldBitmap{nullptr};
    HDC mMemDc{nullptr};
    void* mBits{nullptr};
    HWND findGameWindow() const;
#endif
    int mMapSize{0};
    int mScale{1};
    int mMargin{18};
    bool mVisible{false};
    static long long sClassAtom;
};
}
