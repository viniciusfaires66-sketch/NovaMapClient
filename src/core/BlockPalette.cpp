#include "core/BlockPalette.h"
#include <algorithm>

namespace novamap {
namespace {
bool has(std::string_view id, std::string_view needle) { return id.find(needle) != std::string_view::npos; }
Rgba shade(Rgba c, int delta) {
    auto clamp = [](int v) { return static_cast<std::uint8_t>(std::clamp(v, 0, 255)); };
    c.r = clamp(static_cast<int>(c.r) + delta);
    c.g = clamp(static_cast<int>(c.g) + delta);
    c.b = clamp(static_cast<int>(c.b) + delta);
    return c;
}
}

Rgba colorForBlock(std::string_view id, int y, int neighborY) {
    Rgba c{112, 112, 112, 255};

    if (has(id, "water"))                         c = {52, 92, 170, 255};
    else if (has(id, "lava"))                    c = {220, 85, 25, 255};
    else if (has(id, "grass") || has(id, "moss")) c = {88, 142, 67, 255};
    else if (has(id, "leaves") || has(id, "vine")) c = {64, 122, 55, 255};
    else if (has(id, "sand") || has(id, "sandstone")) c = {219, 204, 143, 255};
    else if (has(id, "snow") || has(id, "ice"))  c = {226, 235, 238, 255};
    else if (has(id, "dirt") || has(id, "mud"))  c = {128, 96, 67, 255};
    else if (has(id, "wood") || has(id, "log") || has(id, "planks")) c = {145, 111, 70, 255};
    else if (has(id, "brick") || has(id, "terracotta")) c = {167, 86, 67, 255};
    else if (has(id, "netherrack"))               c = {113, 54, 54, 255};
    else if (has(id, "end_stone"))                c = {218, 219, 158, 255};
    else if (has(id, "stone") || has(id, "deepslate") || has(id, "ore")) c = {116, 116, 116, 255};

    int relief = 0;
    if (y > neighborY) relief = 10;
    else if (y < neighborY) relief = -10;
    return shade(c, relief);
}
}
