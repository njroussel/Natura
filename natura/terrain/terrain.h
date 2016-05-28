#pragma once

#include <cstdint>
#include <deque>
#include "../grid/grid.h"
#include "../axis/axis.h"
#include "chunk/chunk.h"
#include "chunk/chunk_generation/chunk_factory.h"
#include "../water_grid/water_grid.h"
#include "../skybox/skybox.h"
#include "../config.h"

class Terrain {
public:
    Terrain(uint32_t chunk_per_side, uint32_t quad_side_size, PerlinNoise *perlinNoise) : m_chunk_factory(
            quad_side_size, perlinNoise) {
        for (int i = 0; i < chunk_per_side; i++) {
            std::deque<Chunk *> row;
            for (int j = 0; j < chunk_per_side; j++) {
                row.push_back(m_chunk_factory.createChunk(glm::vec2(i, j)));
            }
            m_chunks.push_back(row);
        }
        m_skybox = new SkyBox();
        m_offset = glm::vec2(0, 0);
        m_perlin_noise = perlinNoise;
        m_axis_pos = glm::vec3(0, 0, 0);
    }

    void Init(GLuint water_reflection_tex) {
        m_water_grid.Init(water_reflection_tex);
        m_axis.Init();
        m_skybox->Init();
        for (size_t i = 0; i < m_chunks.size(); i++) {
            for (size_t j = 0; j < m_chunks[i].size(); j++) {
                m_chunks[i][j]->Init();
            }
        }
    }

    void Draw(float amplitude, float time, glm::vec3 cam_pos, bool onlyTerrain,
              const glm::mat4 &model = IDENTITY_MATRIX,
              const glm::mat4 &view = IDENTITY_MATRIX,
              const glm::mat4 &projection = IDENTITY_MATRIX) {

        m_axis.Draw(glm::translate(model, m_axis_pos), view, projection);
        m_amplitude = amplitude;
        m_skybox->Draw(projection * view * glm::translate(model, -cam_pos / TERRAIN_SCALE));
        glm::mat4 _m = glm::translate(model, glm::vec3(m_offset.x * CHUNK_SIDE_TILE_COUNT, 0,
                                                       m_offset.y * CHUNK_SIDE_TILE_COUNT));
        for (size_t i = 0; i < m_chunks.size(); i++) {
            for (size_t j = 0; j < m_chunks[i].size(); j++) {
                GLuint left = j < m_chunks[i].size() - 1 ? m_chunks[i][j + 1]->getTextureId() : 0;
                GLuint low = i < m_chunks.size() - 1 ? m_chunks[i + 1][j]->getTextureId() : 0;
                GLuint low_left =
                        i < m_chunks.size() - 1 && j < m_chunks[i].size() - 1 ? m_chunks[i + 1][j + 1]->getTextureId()
                                                                              : 0;
                m_chunks[i][j]->Draw(amplitude, time, m_water_height * CHUNK_SIDE_TILE_COUNT, left, low, low_left,
                                     glm::translate(_m, glm::vec3(i * CHUNK_SIDE_TILE_COUNT,
                                                                  0.0, j *
                                                                       CHUNK_SIDE_TILE_COUNT)),
                                     view, projection);
            }
        }
        if (!onlyTerrain) {
            for (size_t i = 0; i < m_chunks.size(); i++) {
                for (size_t j = 0; j < m_chunks.size(); j++) {
                    m_water_grid.Draw(glm::vec2(i * CHUNK_SIDE_TILE_COUNT, j * CHUNK_SIDE_TILE_COUNT), time / 4.0f,
                                      glm::translate(glm::scale(_m, glm::vec3(CHUNK_SIDE_TILE_COUNT)),
                                                     glm::vec3(i, m_water_height, j)), view, projection);
                }
            }
        }
    }

    void Cleanup() {
        for (size_t i = 0; i < m_chunks.size(); i++) {
            for (size_t j = 0; j < m_chunks[i].size(); j++) {
                m_chunks[i][j]->Cleanup();
                delete m_chunks[i][j];
            }
        }
    }

    void ExpandTerrain(glm::vec3 camera_position) {
        const uint32_t edge_threshold = 4;
        /* Redo: Used if the camera is too far away from the terrain so that we need to expand multiple time. */
        bool redo = false;
        //do {
        glm::vec3 cam_pos = camera_position;
        cam_pos = -cam_pos;
        glm::vec3 old = cam_pos;
        cam_pos /= TERRAIN_SCALE;
        cam_pos = getChunkPos(cam_pos);
        redo = true;
        if (cam_pos.z < edge_threshold) {
            _expand(Terrain::Direction::NORTH);
            cout << "EXPANSINO" << endl;
        }
        else if (cam_pos.z > m_chunks[0].size() - 1 - edge_threshold) {
            _expand(Terrain::Direction::SOUTH);
            cout << "EXPANSINO" << endl;
        }
        else if (cam_pos.x < edge_threshold) {
            _expand(Terrain::Direction::WEST);
            cout << "EXPANSINO" << endl;
        }
        else if (cam_pos.x > m_chunks.size() - 1 - edge_threshold) {
            _expand(Terrain::Direction::EST);
            cout << "EXPANSINO" << endl;
        }
        else {
            redo = false;
        }
        //}while (redo);
    }

    float getHeight(glm::vec2 pos) {
        glm::vec3 tmp = glm::vec3(pos.x, 0, pos.y);
        tmp = getChunkPos(tmp);

        glm::vec2 chunk_idx = glm::vec2(tmp.x, tmp.z);
        FrameBuffer *frameBuffer = m_perlin_noise->getFrameBufferForChunk(chunk_idx);
        if(frameBuffer == NULL){
            return NULL;
        }
        glm::vec2 pos_on_tex = pos - glm::vec2((chunk_idx.x + m_offset.x) * CHUNK_SIDE_TILE_COUNT,
                                               (chunk_idx.y + m_offset.y) * CHUNK_SIDE_TILE_COUNT);

        pos_on_tex.x /= (CHUNK_SIDE_TILE_COUNT);
        pos_on_tex.y /= (CHUNK_SIDE_TILE_COUNT);
        pos_on_tex.x *= frameBuffer->getSize().x;
        pos_on_tex.y *= frameBuffer->getSize().y;

        frameBuffer->Bind();
        float height;
        glReadPixels((int) pos_on_tex.x, (int) pos_on_tex.y, 1, 1, GL_RED, GL_FLOAT, &height);
        frameBuffer->Unbind();

        height = (height - 0.5f) * m_amplitude;
        return height;
    }

    enum Direction {
        NORTH, SOUTH, EST, WEST
    };
    float m_water_height = WATER_HEIGHT;
    glm::vec3 m_axis_pos = glm::vec3(0, 0, 0);

private:
    PerlinNoise *m_perlin_noise;
    WaterGrid m_water_grid;
    ChunkFactory m_chunk_factory;
    std::deque<std::deque<Chunk *>> m_chunks;
    SkyBox *m_skybox;
    Axis m_axis;
    glm::vec2 m_offset;
    float m_amplitude;

    glm::vec3 getChunkPos(glm::vec3 pos) {
        pos /= CHUNK_SIDE_TILE_COUNT;
        pos -= glm::vec3(m_offset.x, 0, m_offset.y);
        pos.x = floor(pos.x);
        pos.z = floor(pos.z);
        return pos;
    }

    void _expand(Direction dir) {
        switch (dir) {
            case SOUTH: {
                m_offset.y++;
                m_perlin_noise->setTerrainOffset(m_offset);
                for (int i = 0; i < m_chunks.size(); i++) {
                    m_chunks[i].pop_front();
                    /* NOTE : The +1 in the indice y is important to avoid an off-by-one error. */
                    m_chunks[i].push_back(m_chunk_factory.createChunk(
                            glm::vec2(m_offset.x + i, m_chunks[i].size() - 1 + 1 + m_offset.y)));
                    m_chunks[i][m_chunks[i].size() - 1]->Init();
                }
                break;
            }

            case NORTH: {
                m_offset.y--;
                m_perlin_noise->setTerrainOffset(m_offset);
                for (int i = 0; i < m_chunks.size(); i++) {
                    m_chunks[i].pop_back();
                    m_chunks[i].push_front(m_chunk_factory.createChunk(glm::vec2(m_offset.x + i, m_offset.y)));
                    m_chunks[i][0]->Init();
                }
                break;
            }

            case WEST: {
                m_chunks.pop_back();
                m_chunks.push_front(std::deque<Chunk *>(m_chunks[0].size(), NULL));
                m_offset.x--;
                m_perlin_noise->setTerrainOffset(m_offset);
                for (int i = 0; i < m_chunks[0].size(); i++) {
                    m_chunks[0][i] = m_chunk_factory.createChunk(glm::vec2(m_offset.x, i + m_offset.y));
                    m_chunks[0][i]->Init();
                }
                break;
            }

            case EST: {
                m_chunks.pop_front();
                m_chunks.push_back(std::deque<Chunk *>(m_chunks[0].size(), NULL));
                m_offset.x++;
                m_perlin_noise->setTerrainOffset(m_offset);
                for (int i = 0; i < m_chunks[0].size(); i++) {
                    m_chunks[m_chunks.size() - 1][i] = m_chunk_factory.createChunk(
                            glm::vec2(m_chunks.size() - 1 + m_offset.x, i + m_offset.y));
                    m_chunks[m_chunks.size() - 1][i]->Init();
                }
                break;
            }
        }
    }
};