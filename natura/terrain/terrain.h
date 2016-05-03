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

    void ExpandTerrain(glm::vec3 cam_pos){
        const uint32_t edge_threshold = 2;
        if (cam_pos.x < edge_threshold){
            _expand(Direction::WEST);
        }
        if (cam_pos.x > m_chunks[0].size() - edge_threshold){
            _expand(Direction::EST);
        }
        if (cam_pos.y < edge_threshold){
            _expand(Direction::NORTH);
        }
        if (cam_pos.y > m_chunks.size() - edge_threshold){
            _expand(Direction::SOUTH);
        }
    }

private:
    ChunkFactory m_chunk_factory;
    std::deque<std::deque<Chunk *>> m_chunks;
    Cube* m_skybox;

    enum Direction {NORTH, SOUTH, EST, WEST};

    void _expand(Direction dir){
        switch (dir) {
            case NORTH:
                m_chunks.push_front(std::deque<Chunk*>(m_chunks[0].size(), NULL));
                for (int i = 0 ; i < m_chunks[0].size() ; i ++){
                    m_chunks[0][i] = m_chunk_factory.createChunk(glm::vec2(0, i));
                }
                break;

            case SOUTH:
                m_chunks.push_back(std::deque<Chunk*>(m_chunks[0].size(), NULL));
                for (int i = 0 ; i < m_chunks[0].size() ; i ++){
                    m_chunks[m_chunks.size()-1][i] = m_chunk_factory.createChunk(glm::vec2(0, i));
                }
                break;

            case EST:break;
            case WEST:break;
        }
    }
};