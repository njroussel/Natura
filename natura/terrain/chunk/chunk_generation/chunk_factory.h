#pragma once

#include "../../../perlin_noise/perlinnoise.h"
#include <cstdint>
#include "../chunk.h"


class ChunkFactory {
public:
    ChunkFactory(uint32_t tile_side_size, PerlinNoise *perlin_noise){
        m_tile_side_size = tile_side_size;
        m_perlin_noise = perlin_noise;
    }

    Chunk *createChunk(glm::vec2 indices, Terrain* terrain){
        return new Chunk(indices, m_tile_side_size, m_perlin_noise);
    }

private:
    PerlinNoise *m_perlin_noise;
    uint32_t m_tile_side_size;
};