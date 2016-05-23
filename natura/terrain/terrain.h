#pragma once

#include <cstdint>
#include <deque>
#include "../grid/grid.h"
#include "../axis/axis.h"
#include "chunk/chunk.h"
#include "chunk/chunk_generation/chunk_factory.h"
#include "../water_grid/water_grid.h"
#include "../skybox/cube.h"

#define TERRAIN_SCALE 2.0f
#define WATER_HEIGHT -0.6f

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
        m_axis_pos = glm::vec3(0, 0, 0);
    }

    void Init(){
        m_water_grid.Init();
        m_axis.Init();
        m_skybox->Init();
        for (size_t i = 0 ; i < m_chunks.size() ; i ++) {
            for (size_t j = 0 ; j < m_chunks[i].size() ; j ++) {
                m_chunks[i][j]->Init();
            }
        }
    }

    void Draw(float amplitude, float time, glm::vec3 cam_pos, const glm::mat4 &model = IDENTITY_MATRIX,
              const glm::mat4 &view = IDENTITY_MATRIX,
              const glm::mat4 &projection = IDENTITY_MATRIX) {
        m_amplitude = amplitude;
        //glm::vec3 kek = glm::vec3(abs(20*sin(time/4)), 0, 1);
        //glm::vec3 kek = glm::vec3((4+4*sin(time/4)), 0, 1);
        glm::vec3 kek = glm::vec3(7.9, 0, 7.9);
        kek.y = getHeight(glm::vec2(kek.x, kek.z));
        m_axis.Draw(glm::translate(model, kek), view, projection);
        glm::vec3 tmp;
        m_axis.Draw(glm::translate(model, glm::vec3(0, getHeight(glm::vec2(0, 0)), 0)), view, projection);
        m_axis.Draw(glm::translate(model, glm::vec3(1, getHeight(glm::vec2(1, 0)), 0)), view, projection);
        m_axis.Draw(glm::translate(model, glm::vec3(1, getHeight(glm::vec2(1, 1)), 1)), view, projection);
        m_axis.Draw(glm::translate(model, glm::vec3(0, getHeight(glm::vec2(0, 1)), 1)), view, projection);
        m_skybox->Draw(projection * view * glm::translate(model, -cam_pos/TERRAIN_SCALE));
        glm::mat4 _m = glm::translate(model, glm::vec3(m_offset.x*CHUNK_SIDE_TILE_COUNT, 0, m_offset.y*CHUNK_SIDE_TILE_COUNT));
        for (size_t i = 0 ; i < m_chunks.size() ; i ++) {
            for (size_t j = 0 ; j < m_chunks[i].size() ; j ++) {
                m_chunks[i][j]->Draw(amplitude, time, glm::translate(_m, glm::vec3(i*CHUNK_SIDE_TILE_COUNT, 0.0, j*CHUNK_SIDE_TILE_COUNT)), view, projection);
            }
        }
        for (size_t i = 0 ; i < m_chunks.size() ; i ++) {
            for (size_t j = 0 ; j < m_chunks.size() ; j ++) {
                m_water_grid.Draw(glm::vec2(i*CHUNK_SIDE_TILE_COUNT, j*CHUNK_SIDE_TILE_COUNT), time/8.f, glm::translate(glm::scale(_m, glm::vec3(CHUNK_SIDE_TILE_COUNT)), glm::vec3(i, water_height, j)), view, projection);
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
        /* Camera is not affected by the scale
         * We went through A LOT of pain trying to understand why.
         */
        cam_pos = -cam_pos;
        glm::vec3 old = cam_pos;
        cam_pos /= TERRAIN_SCALE;
        cam_pos = getChunkPos(cam_pos);
        //cout << "real : " << old.x << " , " << old.z << " | cam_pos : " << cam_pos.x << " , " << cam_pos.z << endl;
        if (cam_pos.z < edge_threshold) {
            _expand(Terrain::Direction::NORTH);
            cout << "EXPANSION" << endl;
        }
        else if (cam_pos.z > m_chunks[0].size() - 1 - edge_threshold) {
            _expand(Terrain::Direction::SOUTH);
            cout << "EXPANSION" << endl;
        }
        else if (cam_pos.x < edge_threshold) {
            _expand(Terrain::Direction::WEST);
            cout << "EXPANSION" << endl;
        }
        else if (cam_pos.x > m_chunks.size() - 1 - edge_threshold) {
            _expand(Terrain::Direction::EST);
            cout << "EXPANSION" << endl;
        }
    }

    float getHeight(glm::vec2 pos) {
        //cout << "getHeight : " << pos.x << " " << pos.y << endl;
        //pos -= m_offset;
        glm::vec3 tmp = glm::vec3(pos.x, 0, pos.y);
        tmp = getChunkPos(tmp);
        if (axis___)
            cout << "tmp = " << tmp.x << " " << tmp.z << " |||| ";
        //pos /= TERRAIN_SCALE;
        /*m_axis_pos.x = -pos.x;
        m_axis_pos.z = -pos.y;*/
        //pos = abs(pos);
        glm::vec2 chunk_idx = glm::vec2(tmp.x, tmp.z);
        FrameBuffer *frameBuffer = m_perlin_noise->getFrameBufferForChunk(chunk_idx);
        if (axis___)
            cout << "frameBuffer tex id = " << frameBuffer->getTextureId() << " |||| ";
        //cout << " pos = " << pos.x << " " << pos.y;
        //cout << "  | chunck pos = " << chunk_idx.x << " " << chunk_idx.y;

        glm::vec2 pos_on_tex = pos - glm::vec2((chunk_idx.x + m_offset.x) * CHUNK_SIDE_TILE_COUNT, (chunk_idx.y + m_offset.y)* CHUNK_SIDE_TILE_COUNT);
        /*pos_on_tex.x = abs(pos_on_tex.x);
        pos_on_tex.y = abs(pos_on_tex.y);*/
        pos_on_tex.x /= (CHUNK_SIDE_TILE_COUNT);
        pos_on_tex.y /= (CHUNK_SIDE_TILE_COUNT);
        /*pos_on_tex.x *= TERRAIN_SCALE;
        pos_on_tex.y *= TERRAIN_SCALE;*/
        pos_on_tex.x *= frameBuffer->getSize().x;
        pos_on_tex.y *= frameBuffer->getSize().y;
        if (axis___)
            cout << " pos on tex = " << pos_on_tex.x << " " << pos_on_tex.y << endl;

        frameBuffer->Bind();
        float height;
        glReadPixels((int)pos_on_tex.x, (int)pos_on_tex.y, 1, 1, GL_RED, GL_FLOAT, &height);
        frameBuffer->Unbind();
        //cout << " | height = " << height << endl;
        //m_axis_pos.y = height - 0.5;
        height = (height-0.5f)*m_amplitude;
        water_height = height;
        return height;
    }

    enum Direction {NORTH, SOUTH, EST, WEST};
    float water_height = WATER_HEIGHT;

private:
    glm::vec3 m_axis_pos;
    PerlinNoise *m_perlin_noise;
    WaterGrid m_water_grid;
    ChunkFactory m_chunk_factory;
    std::deque<std::deque<Chunk *>> m_chunks;
    Cube* m_skybox;
    Axis m_axis;
    glm::vec2 m_offset;
    float m_amplitude;

    glm::vec3 getChunkPos(glm::vec3 pos){
        pos /= CHUNK_SIDE_TILE_COUNT;
        pos -= glm::vec3(m_offset.x, 0, m_offset.y);
        pos = abs(pos);
        pos.x = floor(pos.x);
        pos.z = floor(pos.z);
        return pos;
    }
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