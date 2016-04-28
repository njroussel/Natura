#pragma once
#include "../../perlin_noise/perlinnoise.h"
#include "../../../external/glm/detail/type_vec.hpp"
#include "../../../external/glm/detail/type_vec2.hpp"
#include "../../grid/grid.h"
#include "../../../common/icg_helper.h"
#include "../../../external/glm/detail/type_mat.hpp"
#include "../../../external/glm/gtc/matrix_transform.hpp"

#define CHUNK_SIDE_TILE_COUNT 4

class Chunk {
public:
    Chunk(glm::vec2 pos, uint32_t quad_res, PerlinNoise *perlinNoise){
        m_position = pos;
        m_perlin_noise = perlinNoise;
        for (int i = 0 ; i < CHUNK_SIDE_TILE_COUNT ; i ++){
            for (int j = 0 ; j < CHUNK_SIDE_TILE_COUNT ; j ++) {
                m_tiles[i][j] = new Grid(quad_res, glm::vec2(i, j));
            }
        }
    }

    ~Chunk(){
        for (int i = 0 ; i < CHUNK_SIDE_TILE_COUNT ; i ++){
            for (int j = 0 ; j < CHUNK_SIDE_TILE_COUNT ; j ++) {
                //delete m_tiles[i][j];
            }
        }
    }

    void Init(){
        float H = 0.4f;
        float lacunarity = 2.0f;
        float offset = 1.0f;
        float frequency = 0.64f;
        int octaves = 6;
        float amplitude = 0.95f;
        m_chunk_noise_tex_id = m_perlin_noise->generateNoise(glm::vec2(m_position.x, m_position.y));
        for (int i = 0 ; i < CHUNK_SIDE_TILE_COUNT ; i ++) {
            for (int j = 0 ; j < CHUNK_SIDE_TILE_COUNT ; j ++) {
                m_tiles[i][j]->Init(m_chunk_noise_tex_id);
            }
        }
    }

    void Draw(float amplitude, float time, const glm::mat4 &model = IDENTITY_MATRIX,
              const glm::mat4 &view = IDENTITY_MATRIX,
              const glm::mat4 &projection = IDENTITY_MATRIX) {
        for (int i = 0 ; i < CHUNK_SIDE_TILE_COUNT ; i ++){
            for (int j = 0 ; j < CHUNK_SIDE_TILE_COUNT ; j ++) {
                m_tiles[i][j]->Draw(amplitude, time, glm::translate(model, glm::vec3(i, 0, j)), view, projection);
            }
        }
    }

    void Cleanup(){
        for (int i = 0 ; i < CHUNK_SIDE_TILE_COUNT ; i ++) {
            for (int j = 0 ; j < CHUNK_SIDE_TILE_COUNT ; j ++) {
                m_tiles[i][j]->Cleanup();
            }
        }
    }

private:
    glm::vec2 m_position;
    PerlinNoise *m_perlin_noise;
    Grid *m_tiles[CHUNK_SIDE_TILE_COUNT][CHUNK_SIDE_TILE_COUNT];
    int m_chunk_noise_tex_id;
};