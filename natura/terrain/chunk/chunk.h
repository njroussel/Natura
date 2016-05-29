#pragma once

#include "../../perlin_noise/perlinnoise.h"
#include "../../../external/glm/detail/type_vec.hpp"
#include "../../../external/glm/detail/type_vec2.hpp"
#include "../../grid/grid.h"
#include "../../../common/icg_helper.h"
#include "../../../external/glm/detail/type_mat.hpp"
#include "../../../external/glm/gtc/matrix_transform.hpp"
#include "../terrain.h"

#define CHUNK_SIDE_TILE_COUNT 4
#include "../../config.h"


class Chunk : public Observer {
public:
    Chunk(glm::vec2 pos, uint32_t quad_res, PerlinNoise *perlinNoise) {
        m_position = pos;
        m_perlin_noise = perlinNoise;
   }

    ~Chunk() { }

    void Init() {
        m_perlin_noise->attach(this);
        m_chunk_noise_tex_id = m_perlin_noise->generateNoise(glm::vec2(m_position.x, m_position.y));
    }

    void Draw(float amplitude, float time, float water_height, GLuint left_tex, GLuint low_tex, GLuint low_left_tex,
              const glm::mat4 &model = IDENTITY_MATRIX,
              const glm::mat4 &view = IDENTITY_MATRIX,
              const glm::mat4 &projection = IDENTITY_MATRIX) {
        for (int i = 0; i < CHUNK_SIDE_TILE_COUNT; i++) {
            for (int j = 0; j < CHUNK_SIDE_TILE_COUNT; j++) {
                BASE_TILE->setTextureId(m_chunk_noise_tex_id);
                BASE_TILE->setTextureLeft(left_tex);
                BASE_TILE->setTextureLow(low_tex);
                BASE_TILE->setTextureLowLeft(low_left_tex);
                BASE_TILE->Draw(glm::vec2(i, j), amplitude, water_height, time, glm::translate(model, glm::vec3(i, 0, j)), view, projection);

            }
        }
    }

    void Cleanup() {

    }

    virtual void update(Message *msg) {
        if (msg->getType() == Message::Type::PERLIN_PROP_CHANGE) {
            //glDeleteTextures(1, (GLuint *) (&m_chunk_noise_tex_id));
            m_chunk_noise_tex_id = m_perlin_noise->generateNoise(m_position);
        }
        else {
            throw std::string("Error unexpected message type.");
        }
    }

    glm::vec2 getPosition() {
        return m_position;
    }

    int getTextureId() {
        return m_chunk_noise_tex_id;
    }

private:
    glm::vec2 m_position;
    PerlinNoise *m_perlin_noise;
    int m_chunk_noise_tex_id;
};