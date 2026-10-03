#include "core/MinimapRenderer.h"
#include <algorithm>
#include <cmath>

namespace novamap {
namespace {
constexpr std::uint32_t parchmentDark = 0xFFD2B98Bu;
constexpr std::uint32_t parchmentMid  = 0xFFE7D2A0u;
constexpr std::uint32_t parchmentLite = 0xFFF2E2B7u;
constexpr std::uint32_t unknown       = 0xE0282622u;
constexpr std::uint32_t white         = 0xFFFFFFFFu;
constexpr std::uint32_t markerOutline = 0xFF202020u;
}

MinimapRenderer::MinimapRenderer(int mapSize, int border)
: mMapSize(mapSize), mBorder(border), mOutputSize(mapSize + border * 2),
  mPixels(static_cast<std::size_t>(mOutputSize * mOutputSize), unknown) {}

std::uint32_t MinimapRenderer::pack(Rgba c) {
    return (static_cast<std::uint32_t>(c.a) << 24) |
           (static_cast<std::uint32_t>(c.r) << 16) |
           (static_cast<std::uint32_t>(c.g) << 8) |
           static_cast<std::uint32_t>(c.b);
}

void MinimapRenderer::put(int x, int y, std::uint32_t color) {
    if (x >= 0 && y >= 0 && x < mOutputSize && y < mOutputSize)
        mPixels[static_cast<std::size_t>(y * mOutputSize + x)] = color;
}

void MinimapRenderer::compose(TileCache const& cache, PlayerState const& player) {
    std::fill(mPixels.begin(), mPixels.end(), parchmentDark);

    for (int y = 1; y < mOutputSize - 1; ++y)
        for (int x = 1; x < mOutputSize - 1; ++x)
            put(x, y, parchmentMid);
    for (int y = 3; y < mOutputSize - 3; ++y)
        for (int x = 3; x < mOutputSize - 3; ++x)
            put(x, y, parchmentLite);

    const int ox = mBorder;
    const int oy = mBorder;
    for (int y = 0; y < mMapSize; ++y)
        for (int x = 0; x < mMapSize; ++x)
            put(ox + x, oy + y, unknown);

    if (!player.valid) return;

    int half = mMapSize / 2;
    int px = static_cast<int>(std::floor(player.x));
    int pz = static_cast<int>(std::floor(player.z));

    for (int sy = 0; sy < mMapSize; ++sy) {
        for (int sx = 0; sx < mMapSize; ++sx) {
            int wx = px + (sx - half);
            int wz = pz + (sy - half);
            auto cell = cache.findCell(player.dimension, wx, wz);
            if (cell && cell->known) put(ox + sx, oy + sy, pack(cell->color));
        }
    }

    int cx = ox + half;
    int cy = oy + half;
    put(cx, cy - 5, markerOutline);
    put(cx - 1, cy - 4, markerOutline); put(cx, cy - 4, white); put(cx + 1, cy - 4, markerOutline);
    for (int dx = -2; dx <= 2; ++dx) put(cx + dx, cy - 3, dx == 0 ? white : markerOutline);
    put(cx - 1, cy - 2, markerOutline); put(cx, cy - 2, white); put(cx + 1, cy - 2, markerOutline);
    put(cx, cy - 1, white); put(cx, cy, white); put(cx, cy + 1, white); put(cx, cy + 2, markerOutline);
}

} // namespace novamap
