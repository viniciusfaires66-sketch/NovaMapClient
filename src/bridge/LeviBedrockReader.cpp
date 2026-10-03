#include "bridge/LeviBedrockReader.h"

#include "ll/api/service/TargetedBedrock.h"
#include "mc/client/game/ClientInstance.h"
#include "mc/client/player/LocalPlayer.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/block/Block.h"

#include <cmath>

namespace novamap {

PlayerState LeviBedrockReader::getPlayerState() {
    auto client = ll::service::getClientInstance();
    if (!client) return {};

    auto* player = client->getLocalPlayer();
    if (!player) return {};

    auto const& pos = player->getPosition();
    PlayerState state{};
    state.x = pos.x;
    state.y = pos.y;
    state.z = pos.z;
    state.yaw = 0.0f;
    state.dimension = static_cast<int>(player->getDimensionId());
    state.valid = true;
    return state;
}

SurfaceSample LeviBedrockReader::sampleSurface(int dimension, int x, int z) {
    auto client = ll::service::getClientInstance();
    if (!client) return {};

    auto* player = client->getLocalPlayer();
    if (!player || static_cast<int>(player->getDimensionId()) != dimension) return {};

    auto& region = player->getDimensionBlockSource();

    BlockPos probe{x, static_cast<int>(std::floor(player->getPosition().y)), z};
    if (!region.areChunksFullyLoaded(probe, 0)) return {};

    BlockPos top = region.getHeightmapPos(BlockPos{x, 0, z});

    BlockPos surface = top;
    auto const* block = &region.getBlock(surface);
    for (int i = 0; i < 8 && block->isAir(); ++i) {
        --surface.y;
        block = &region.getBlock(surface);
    }

    SurfaceSample out{};
    out.blockId = block->getTypeName();
    out.y = surface.y;
    out.valid = !block->isAir() && !out.blockId.empty();
    return out;
}

} // namespace novamap
