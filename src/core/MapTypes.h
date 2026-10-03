#pragma once
#include <cstdint>
#include <string>

namespace novamap {

struct Rgba {
    std::uint8_t r{0}, g{0}, b{0}, a{255};
};

struct PlayerState {
    double x{0.0};
    double y{0.0};
    double z{0.0};
    float yaw{0.0f};
    int dimension{0};
    bool valid{false};
};

struct SurfaceSample {
    std::string blockId;
    int y{0};
    bool valid{false};
};

struct TileKey {
    int dimension{0};
    int x{0};
    int z{0};
    bool operator==(TileKey const&) const = default;
};

struct TileKeyHash {
    std::size_t operator()(TileKey const& k) const noexcept {
        std::uint64_t a = static_cast<std::uint32_t>(k.x);
        std::uint64_t b = static_cast<std::uint32_t>(k.z);
        std::uint64_t c = static_cast<std::uint32_t>(k.dimension);
        return static_cast<std::size_t>((a * 0x9E3779B185EBCA87ULL) ^ (b << 1) ^ (c << 33));
    }
};

} // namespace novamap
