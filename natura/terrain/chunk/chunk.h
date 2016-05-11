#pragma once
#include "../../perlin_noise/perlinnoise.h"
#include "../../../external/glm/detail/type_vec.hpp"
#include "../../../external/glm/detail/type_vec2.hpp"
#include "../../grid/grid.h"
#include "../../../common/icg_helper.h"
#include "../../../external/glm/detail/type_mat.hpp"
#include "../../../external/glm/gtc/matrix_transform.hpp"

#define CHUNK_SIDE_TILE_COUNT 4


class Chunk : public Observer{
public:
    Chunk(glm::vec2 pos, uint32_t quad_res, PerlinNoise *perlinNoise) : m_base_tile(quad_res, glm::vec2(0, 0)){
        m_position = pos;
        m_perlin_noise = perlinNoise;
    }

    ~Chunk(){}

    void Init(){
        m_perlin_noise->attach(this);
        m_chunk_noise_tex_id = m_perlin_noise->generateNoise(glm::vec2(m_position.x, m_position.y));
        m_base_tile.Init(m_chunk_noise_tex_id);
    }

    void Draw(float amplitude, float time, const glm::mat4 &model = IDENTITY_MATRIX,
              const glm::mat4 &view = IDENTITY_MATRIX,
              const glm::mat4 &projection = IDENTITY_MATRIX) {
        for (int i = 0 ; i < CHUNK_SIDE_TILE_COUNT ; i ++){
            for (int j = 0 ; j < CHUNK_SIDE_TILE_COUNT ; j ++) {
                m_base_tile.Draw(glm::vec2(i, j), amplitude, time, glm::translate(model, glm::vec3(i, 0, j)), view, projection);
            }
        }
    }

    void Cleanup(){
        m_base_tile.Cleanup();
    }

    virtual void update(Message *msg){
        if (msg->getType() == Message::Type::PERLIN_PROP_CHANGE) {
            glDeleteTextures(1, (GLuint *) (&m_chunk_noise_tex_id));
            m_chunk_noise_tex_id = m_perlin_noise->generateNoise(m_position);
            m_base_tile.setTextureId(m_chunk_noise_tex_id);
        }
        else {
            throw std::string("Error unexpected message type.");
        }
    }

    glm::vec2 getPosition() {
        return m_position;
    }

private:
    Grid m_base_tile;
    glm::vec2 m_position;
    PerlinNoise *m_perlin_noise;
    int m_chunk_noise_tex_id;
};