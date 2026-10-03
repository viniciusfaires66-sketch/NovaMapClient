#pragma once
#include "core/TileCache.h"
#include <vector>

namespace novamap {
class MinimapRenderer {
public:
    explicit MinimapRenderer(int mapSize = 128, int border = 6);
    void compose(TileCache const& cache, PlayerState const& player);
    int size() const { return mOutputSize; }
    int mapSize() const { return mMapSize; }
    std::vector<std::uint32_t> const& pixels() const { return mPixels; }
private:
    int mMapSize;
    int mBorder;
    int mOutputSize;
    std::vector<std::uint32_t> mPixels;
    static std::uint32_t pack(Rgba c);
    void put(int x, int y, std::uint32_t color);
};
}
