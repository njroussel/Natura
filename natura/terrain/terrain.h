#pragma once

#include <cstdint>
#include <deque>
#include "../grid/grid.h"
#include "chunk/chunk.h"
#include "chunk/chunk_generation/chunk_factory.h"


class Terrain {
public:
    Terrain(uint32_t chunk_per_side, uint32_t quad_side_size, PerlinNoise *perlinNoise) : m_chunk_factory(quad_side_size, perlinNoise){
        for (int i = 0 ; i < chunk_per_side; i ++) {
            std::deque<Chunk *> row;
            for (int j = 0 ; j < chunk_per_side; j ++) {
                row.push_back(m_chunk_factory.createChunk(glm::vec2(i, j)));
            }
            m_chunks.push_back(row);
        }
        m_skybox = new Cube();
    }

    void Init(){
        m_skybox->Init();
        for (size_t i = 0 ; i < m_chunks.size() ; i ++) {
            for (size_t j = 0 ; j < m_chunks.size() ; j ++) {
                m_chunks[i][j]->Init();
            }
        }
    }

    void Draw(float amplitude, float time, const glm::mat4 &model = IDENTITY_MATRIX,
              const glm::mat4 &view = IDENTITY_MATRIX,
              const glm::mat4 &projection = IDENTITY_MATRIX) {
        m_skybox->Draw(projection * view * model);
        for (size_t i = 0 ; i < m_chunks.size() ; i ++) {
            for (size_t j = 0 ; j < m_chunks.size() ; j ++) {
                m_chunks[i][j]->Draw(amplitude, time, glm::translate(model, glm::vec3(i*CHUNK_SIDE_TILE_COUNT, 0.0, j*CHUNK_SIDE_TILE_COUNT)), view, projection);
            }
        }
    }

    void Cleanup(){
        for (size_t i = 0 ; i < m_chunks.size() ; i ++) {
            for (size_t j = 0 ; j < m_chunks.size() ; j ++) {
                m_chunks[i][j]->Cleanup();
                delete m_chunks[i][j];
            }
        }
    }

private:
    ChunkFactory m_chunk_factory;
    std::deque<std::deque<Chunk *>> m_chunks;
    Cube* m_skybox;
};