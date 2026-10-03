#pragma once
#include "core/MapTypes.h"

namespace novamap {
class IWorldReader {
public:
    virtual ~IWorldReader() = default;
    virtual PlayerState getPlayerState() = 0;
    virtual SurfaceSample sampleSurface(int dimension, int x, int z) = 0;
};
}
