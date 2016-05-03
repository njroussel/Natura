#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>
#include <cstdint>

struct DIRECTION {
    enum ENUM {
        Forward = 0, Backward = 1, Left = 2, Right = 3
    };
};


class Camera {

public:

    Camera(vec3 &starting_position, vec2 &starting_rotation) {
        m_position = vec3(starting_position.x, starting_position.y, starting_position.z);
        m_rotation = vec2(starting_rotation.x, starting_rotation.y);
        m_matrix = IDENTITY_MATRIX;

        for (int i = 0; i < m_moving_size; i++) {
            m_moving[i] = false;
        }
    }

    ~Camera() {
    }

    void CalculateMatrix() {
        ComputeMovement();

        m_matrix = IDENTITY_MATRIX;
        m_matrix = glm::rotate(m_matrix, radians(m_rotation.x), vec3(1.0f, 0.0f, 0.0f));
        m_matrix = glm::rotate(m_matrix, radians(m_rotation.y), vec3(0.0f, 1.0f, 0.0f));
        m_matrix = glm::translate(m_matrix, m_position);
    }

    mat4& GetMatrix() {
        return m_matrix;
    }

    void SetRotation(vec2 &new_rotation) {
        m_rotation = new_rotation;
    }

    void AddRotation(vec2 &rotation) {
        m_rotation += rotation;
    }

    void SetMovement(DIRECTION::ENUM direction, bool boolean) {
        m_moving[direction] = boolean;
    }

private:
    const size_t m_moving_size = 4;
    bool m_moving[4];
    vec3 m_position;
    vec2 m_rotation;
    mat4 m_matrix;
    float m_movement_factor = 0.01f;

    void ComputeMovement() {
        vec3 forward_direction = normalize(vec3(-cos(radians(m_rotation.x)) * sin(radians(m_rotation.y)),
                                                sin(radians(m_rotation.x)) * cos(radians(m_rotation.x)),
                                                cos(radians(m_rotation.x)) * cos(radians(m_rotation.y))));

        vec3 left_direction = vec3(cos(radians(m_rotation.y)),
                                   0,
                                   sin(radians(m_rotation.y)));;

        if (m_moving[DIRECTION::ENUM::Forward]) {
            m_position += m_movement_factor * forward_direction;
        }
        if (m_moving[DIRECTION::ENUM::Backward]) {
            m_position += m_movement_factor * -forward_direction;
        }
        if (m_moving[DIRECTION::ENUM::Left]) {
            m_position += m_movement_factor * left_direction;
        }
        if (m_moving[DIRECTION::ENUM::Right]) {
            m_position += m_movement_factor * -left_direction;
        }
    }
};

