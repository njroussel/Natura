#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "icg_helper.h"
#include "../physics/material_point.h"
#include "../terrain/terrain.h"
#include <glm/gtc/type_ptr.hpp>
#include <cstdint>
#include "camera.h"

class FPSCamera {
public:
    FPSCamera(glm::vec3 init_pos, Terrain *terrain){
        m_terrain = terrain;
        m_position = init_pos;
        m_rotation = glm::vec2(0, 0);
        m_moving[0] = false;
        m_moving[1] = false;
        m_moving[2] = false;
        m_moving[3] = false;
    }

    void CalculateMatrix() {
        ComputeMovement();

        m_matrix = IDENTITY_MATRIX;
        m_matrix = glm::rotate(m_matrix, radians(m_rotation.x), vec3(1.0f, 0.0f, 0.0f));
        m_matrix = glm::rotate(m_matrix, radians(m_rotation.y), vec3(0.0f, 1.0f, 0.0f));
        m_matrix = glm::translate(m_matrix, m_position);
    }

    glm::mat4 &GetMatrix() {
        return m_matrix;
    }

    void SetRotation(vec2 &new_rotation) {
        m_rotation = new_rotation;
    }

    void AddRotation(vec2 &rotation) {
        m_rotation += rotation;
        cout << m_rotation.x << "   " << m_rotation.y << endl;
    }

    void SetMovement(DIRECTION direction, bool boolean) {
        m_moving[direction] = boolean;
    }

    glm::vec3 getPosition() {
        return m_position;
    }

    glm::vec3 m_position;
private:
    static const size_t m_moving_size = 4;
    bool m_moving[m_moving_size];
    Terrain *m_terrain;
    glm::vec2 m_rotation;
    glm::mat4 m_matrix;
    float m_movement_factor = 0.4f;

    void ComputeMovement() {
        vec3 forward_direction = normalize(vec3(-cos(radians(m_rotation.x)) * sin(radians(m_rotation.y)),
                                                sin(radians(m_rotation.x)),
                                                cos(radians(m_rotation.x)) * cos(radians(m_rotation.y))));
        forward_direction.y = 0;
        forward_direction = normalize(forward_direction);
        vec3 left_direction = vec3(cos(radians(m_rotation.y)),
                                   0,
                                   sin(radians(m_rotation.y)));

        if (m_moving[DIRECTION::Forward]) {
            m_position += m_movement_factor * forward_direction;
        }
        if (m_moving[DIRECTION::Backward]) {
            m_position += m_movement_factor * -forward_direction;
        }
        if (m_moving[DIRECTION::Left]) {
            m_position += m_movement_factor * left_direction;
        }
        if (m_moving[DIRECTION::Right]) {
            m_position += m_movement_factor * -left_direction;
        }

        m_position.y = -1 * TERRAIN_SCALE * m_terrain->getHeight(glm::vec2(-m_position.x/TERRAIN_SCALE, -m_position.z/TERRAIN_SCALE));

        cout << "Cam pos = " << m_position.x << " " << m_position.y << " " << m_position.z << endl;
    }
};