#pragma once

#include <cstdint>
#include <deque>
#include "../grid/grid.h"


class Terrain {
public:
    Terrain(uint32_t side_quad_count, uint32_t quad_side_size) {
        for (size_t i = 0 ; i < side_quad_count ; i ++) {
            std::deque<Grid> row;
            for (size_t j = 0 ; j < side_quad_count ; j ++) {
                Grid grid(quad_side_size);
                row.push_back(grid);
            }
            m_quads.push_back(row);
        }
    }

    void Init(int tex){
        for (size_t i = 0 ; i < m_quads.size() ; i ++) {
            for (size_t j = 0 ; j < m_quads.size() ; j ++) {
                m_quads[i][j].Init(tex);
            }
        }
    }

    /**
     * Refresh the terrain grid, takes the new texture Id of the new perlin noise.
     */
    void Refresh(int tex_id){
        for (size_t i = 0 ; i < m_quads.size() ; i ++) {
            for (size_t j = 0 ; j < m_quads.size() ; j ++) {
                m_quads[i][j].setTextureId(tex_id);
            }
        }
    }

    void Draw(float amplitude, float time, const glm::mat4 &model = IDENTITY_MATRIX,
              const glm::mat4 &view = IDENTITY_MATRIX,
              const glm::mat4 &projection = IDENTITY_MATRIX) {
        for (size_t i = 0 ; i < m_quads.size() ; i ++) {
            for (size_t j = 0 ; j < m_quads.size() ; j ++) {
                m_quads[i][j].Draw(amplitude, time, glm::translate(model, glm::vec3(i, 0, j)), view, projection);
            }
        }
    }

    void Cleanup(){
        for (size_t i = 0 ; i < m_quads.size() ; i ++) {
            for (size_t j = 0 ; j < m_quads.size() ; j ++) {
                m_quads[i][j].Cleanup();
            }
        }
    }

private:
    std::deque<std::deque<Grid>> m_quads;
};