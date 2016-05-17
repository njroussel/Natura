#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "icg_helper.h"
#include "../physics/material_point.h"
#include <glm/gtc/type_ptr.hpp>
#include <cstdint>

struct DIRECTION {
    enum ENUM {
        Forward = 0, Backward = 1, Left = 2, Right = 3
    };
};


class Camera : public MaterialPoint{

public:

    Camera(vec3 &starting_position, vec2 &starting_rotation) : MaterialPoint(1.0, starting_position){
        m_rotation = vec2(starting_rotation.x, starting_rotation.y);
        m_matrix = IDENTITY_MATRIX;

        for (int i = 0; i < m_moving_size; i++) {
            m_moving[i] = false;
        }
    }

    ~Camera() {
    }

    void CalculateMatrix() {
        //ComputeMovement();

        m_matrix = IDENTITY_MATRIX;
        m_matrix = glm::rotate(m_matrix, radians(m_rotation.x), vec3(1.0f, 0.0f, 0.0f));
        m_matrix = glm::rotate(m_matrix, radians(m_rotation.y), vec3(0.0f, 1.0f, 0.0f));
        m_matrix = glm::translate(m_matrix, getPosition());
    }

    mat4 &GetMatrix() {
        return m_matrix;
    }

    void SetRotation(vec2 &new_rotation) {
        m_rotation = new_rotation;
        vec3 forward_direction = normalize(vec3(-cos(radians(m_rotation.x)) * sin(radians(m_rotation.y)),
                                                sin(radians(m_rotation.x)),
                                                cos(radians(m_rotation.x)) * cos(radians(m_rotation.y))));

        setAccelerationVector(forward_direction);
    }

    void AddRotation(vec2 &rotation) {
        m_rotation += rotation;
        vec3 forward_direction = normalize(vec3(-cos(radians(m_rotation.x)) * sin(radians(m_rotation.y)),
                                                sin(radians(m_rotation.x)),
                                                cos(radians(m_rotation.x)) * cos(radians(m_rotation.y))));

        setAccelerationVector(forward_direction);
    }

    void SetMovement(DIRECTION::ENUM direction, bool boolean) {
        vec3 forward_direction = normalize(vec3(-cos(radians(m_rotation.x)) * sin(radians(m_rotation.y)),
                                                sin(radians(m_rotation.x)),
                                                cos(radians(m_rotation.x)) * cos(radians(m_rotation.y))));

        if (boolean)
            setAccelerationVector(forward_direction);
        m_moving[direction] = boolean;
    }

private:
    static const size_t m_moving_size = 4;
    bool m_moving[m_moving_size];
    vec2 m_rotation;
    mat4 m_matrix;
    float m_movement_factor = 0.4f;
};

