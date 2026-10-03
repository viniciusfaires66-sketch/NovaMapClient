#pragma once
#include "core/MapTypes.h"
#include <string_view>

namespace novamap {
Rgba colorForBlock(std::string_view id, int y, int neighborY);
}
