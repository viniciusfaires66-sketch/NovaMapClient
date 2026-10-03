#pragma once
#include "core/IWorldReader.h"

namespace novamap {
class LeviBedrockReader final : public IWorldReader {
public:
    PlayerState getPlayerState() override;
    SurfaceSample sampleSurface(int dimension, int x, int z) override;
};
}
