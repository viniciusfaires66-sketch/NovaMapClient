#pragma once
#include "core/IWorldReader.h"
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace novamap {

class TileCache {
public:
    static constexpr int TileSize = 128;

    struct Cell {
        Rgba color{0,0,0,0};
        std::int16_t height{0};
        bool known{false};
    };

    struct Tile {
        TileKey key{};
        std::vector<Cell> cells;
        std::size_t scanCursor{0};
        Tile() : cells(TileSize * TileSize) {}
    };

    Tile& getOrCreate(TileKey const& key);
    Cell const* findCell(int dimension, int worldX, int worldZ) const;
    Cell* findCellMutable(int dimension, int worldX, int worldZ);
    void scanBudget(IWorldReader& world, PlayerState const& player, int budget);
    void clear();

private:
    std::unordered_map<TileKey, Tile, TileKeyHash> mTiles;
    static int floorDiv(int v, int d);
    static int floorMod(int v, int d);
};

} // namespace novamap
