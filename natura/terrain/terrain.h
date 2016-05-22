#pragma once

#include <cstdint>
#include <deque>
#include "../grid/grid.h"
#include "../axis/axis.h"
#include "chunk/chunk.h"
#include "chunk/chunk_generation/chunk_factory.h"
#include "../water_grid/water_grid.h"

#define TERRAIN_SCALE 2.0f
#define WATER_HEIGHT -0.0f

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
        m_offset = glm::vec2(0, 0);
        m_perlin_noise = perlinNoise;
    }

    void Init(GLuint water_reflection_tex){
        m_water_grid.Init(water_reflection_tex);
        m_axis.Init();
        m_skybox->Init();
        for (size_t i = 0 ; i < m_chunks.size() ; i ++) {
            for (size_t j = 0 ; j < m_chunks[i].size() ; j ++) {
                m_chunks[i][j]->Init();
            }
        }
    }

    void Draw(float amplitude, float time, glm::vec3 cam_pos, bool onlyTerrain, const glm::mat4 &model = IDENTITY_MATRIX,
              const glm::mat4 &view = IDENTITY_MATRIX,
              const glm::mat4 &projection = IDENTITY_MATRIX) {
        m_axis.Draw(model, view, projection);
        glm::mat4 _m = glm::translate(model, glm::vec3(m_offset.x*CHUNK_SIDE_TILE_COUNT, 0, m_offset.y*CHUNK_SIDE_TILE_COUNT));
        for (size_t i = 0 ; i < m_chunks.size() ; i ++) {
            for (size_t j = 0 ; j < m_chunks[i].size() ; j ++) {
                m_chunks[i][j]->Draw(amplitude, time, glm::translate(_m, glm::vec3(i*CHUNK_SIDE_TILE_COUNT, 0.0, j*CHUNK_SIDE_TILE_COUNT)), view, projection);
            }
        }
        m_skybox->Draw(projection * view * glm::translate(model, -cam_pos/TERRAIN_SCALE));
        if(!onlyTerrain) {
            for (size_t i = 0; i < m_chunks.size(); i++) {
                for (size_t j = 0; j < m_chunks.size(); j++) {
                    m_water_grid.Draw(glm::vec2(i * CHUNK_SIDE_TILE_COUNT, j * CHUNK_SIDE_TILE_COUNT), time / 8.f,
                                      glm::translate(glm::scale(_m, glm::vec3(CHUNK_SIDE_TILE_COUNT)),
                                                     glm::vec3(i, water_height, j)), view, projection);
                }
            }
        }
    }

    void Cleanup(){
        for (size_t i = 0 ; i < m_chunks.size() ; i ++) {
            for (size_t j = 0 ; j < m_chunks[i].size() ; j ++) {
                m_chunks[i][j]->Cleanup();
                delete m_chunks[i][j];
            }
        }
    }

    void ExpandTerrain(glm::vec3 cam_pos){
        const uint32_t edge_threshold = 1;
        cam_pos /= CHUNK_SIDE_TILE_COUNT;
        cam_pos /= TERRAIN_SCALE;
        cam_pos += glm::vec3(m_offset.x, 0, m_offset.y);
        cam_pos = abs(cam_pos);
        cam_pos.x = (int) cam_pos.x;
        cam_pos.z = (int) cam_pos.z;
        //cout << "cam_pos : " << cam_pos.x << " , " << cam_pos.z << endl;
        if (cam_pos.z < edge_threshold)
            _expand(Terrain::Direction::NORTH);
        else if (cam_pos.z > m_chunks[0].size() - 1 - edge_threshold)
            _expand(Terrain::Direction::SOUTH);
        else if (cam_pos.x < edge_threshold)
            _expand(Terrain::Direction::WEST);
        else if (cam_pos.x > m_chunks.size() - 1 - edge_threshold)
            _expand(Terrain::Direction::EST);
    }

    enum Direction {NORTH, SOUTH, EST, WEST};
    float water_height = WATER_HEIGHT;

private:
    PerlinNoise *m_perlin_noise;
    WaterGrid m_water_grid;
    ChunkFactory m_chunk_factory;
    std::deque<std::deque<Chunk *>> m_chunks;
    Cube* m_skybox;
    Axis m_axis;
    glm::vec2 m_offset;

    void _expand(Direction dir){
        switch (dir) {
            case SOUTH: {
                m_offset.y ++;
                m_perlin_noise->setTerrainOffset(m_offset);
                for (int i = 0 ; i < m_chunks.size() ; i ++){
                    m_chunks[i].pop_front();
                    /* NOTE : The +1 in the indice y is important to avoid an off-by-one error. */
                    m_chunks[i].push_back(m_chunk_factory.createChunk(glm::vec2(m_offset.x + i, m_chunks[i].size()-1 + 1 + m_offset.y)));
                    m_chunks[i][m_chunks[i].size()-1]->Init();
                }
                break;
            }

            case NORTH: {
                m_offset.y --;
                m_perlin_noise->setTerrainOffset(m_offset);
                for (int i = 0 ; i < m_chunks.size() ; i ++){
                    m_chunks[i].pop_back();
                    m_chunks[i].push_front(m_chunk_factory.createChunk(glm::vec2(m_offset.x + i, m_offset.y)));
                    m_chunks[i][0]->Init();
                }
                break;
            }

            case WEST: {
                m_chunks.pop_back();
                m_chunks.push_front(std::deque<Chunk *>(m_chunks[0].size(), NULL));
                m_offset.x --;
                m_perlin_noise->setTerrainOffset(m_offset);
                for (int i = 0 ; i < m_chunks[0].size() ; i ++){
                    m_chunks[0][i] = m_chunk_factory.createChunk(glm::vec2(m_offset.x, i + m_offset.y));
                    m_chunks[0][i]->Init();
                }
                break;
            }

            case EST: {
                m_chunks.pop_front();
                m_chunks.push_back(std::deque<Chunk *>(m_chunks[0].size(), NULL));
                m_offset.x ++;
                m_perlin_noise->setTerrainOffset(m_offset);
                for (int i = 0 ; i < m_chunks[0].size() ; i ++){
                    m_chunks[m_chunks.size()-1][i] = m_chunk_factory.createChunk(glm::vec2(m_chunks.size()-1 + m_offset.x, i + m_offset.y));
                    m_chunks[m_chunks.size()-1][i]->Init();
                }
                break;
            }
        }
    }
};