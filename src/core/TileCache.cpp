#include "core/TileCache.h"
#include "core/BlockPalette.h"
#include <algorithm>
#include <cmath>

namespace novamap {

int TileCache::floorDiv(int v, int d) {
    int q = v / d;
    int r = v % d;
    if (r != 0 && ((r < 0) != (d < 0))) --q;
    return q;
}
int TileCache::floorMod(int v, int d) {
    int r = v % d;
    return r < 0 ? r + std::abs(d) : r;
}

TileCache::Tile& TileCache::getOrCreate(TileKey const& key) {
    auto [it, inserted] = mTiles.try_emplace(key);
    if (inserted) it->second.key = key;
    return it->second;
}

TileCache::Cell const* TileCache::findCell(int dimension, int worldX, int worldZ) const {
    int tx = floorDiv(worldX, TileSize);
    int tz = floorDiv(worldZ, TileSize);
    auto it = mTiles.find(TileKey{dimension, tx, tz});
    if (it == mTiles.end()) return nullptr;
    int lx = floorMod(worldX, TileSize);
    int lz = floorMod(worldZ, TileSize);
    return &it->second.cells[static_cast<std::size_t>(lz * TileSize + lx)];
}

TileCache::Cell* TileCache::findCellMutable(int dimension, int worldX, int worldZ) {
    int tx = floorDiv(worldX, TileSize);
    int tz = floorDiv(worldZ, TileSize);
    auto& tile = getOrCreate(TileKey{dimension, tx, tz});
    int lx = floorMod(worldX, TileSize);
    int lz = floorMod(worldZ, TileSize);
    return &tile.cells[static_cast<std::size_t>(lz * TileSize + lx)];
}

void TileCache::scanBudget(IWorldReader& world, PlayerState const& player, int budget) {
    if (!player.valid || budget <= 0) return;

    const int px = static_cast<int>(std::floor(player.x));
    const int pz = static_cast<int>(std::floor(player.z));
    const int baseTx = floorDiv(px, TileSize);
    const int baseTz = floorDiv(pz, TileSize);

    static constexpr int order[9][2] = {
        {0,0}, {0,-1}, {1,0}, {0,1}, {-1,0}, {1,-1}, {1,1}, {-1,1}, {-1,-1}
    };

    for (auto const& off : order) {
        if (budget <= 0) break;
        auto& tile = getOrCreate(TileKey{player.dimension, baseTx + off[0], baseTz + off[1]});
        for (std::size_t tries = 0; tries < tile.cells.size() && budget > 0; ++tries) {
            std::size_t i = tile.scanCursor++ % tile.cells.size();
            auto& cell = tile.cells[i];
            if (cell.known) continue;

            int lx = static_cast<int>(i % TileSize);
            int lz = static_cast<int>(i / TileSize);
            int wx = tile.key.x * TileSize + lx;
            int wz = tile.key.z * TileSize + lz;
            auto s = world.sampleSurface(player.dimension, wx, wz);
            --budget;
            if (!s.valid) continue;

            int neighborY = s.y;
            if (auto const* n = findCell(player.dimension, wx, wz - 1); n && n->known) neighborY = n->height;
            cell.height = static_cast<std::int16_t>(std::clamp(s.y, -32768, 32767));
            cell.color = colorForBlock(s.blockId, s.y, neighborY);
            cell.known = true;
        }
    }
}

void TileCache::clear() { mTiles.clear(); }

} // namespace novamap
