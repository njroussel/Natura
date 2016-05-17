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

#define print_vec3(vec)\
    printf(#vec " = %f ; %f ; %f\n", vec.x, vec.y, vec.z)

#define print_vec2(vec)\
    printf(#vec " = %f ; %f\n", vec.x, vec.y)

    void LookAt(glm::vec3 target) {
        glm::vec3 pos = - getPosition();
        /*float dx = target.x - pos.x;
        float dz = target.z - pos.z;
        float dy = target.y - pos.y;
        //m_rotation.y = atan(dx/dz);
        //m_rotation.x = atan(dy/dz);
        cout << "Pos = " << pos.x << "    " << pos.y << "    " << pos.z << endl;

        glm::vec3 forw = -getForwardDirection();
        glm::vec3 ray = target - pos;

        glm::vec2 fyz = normalize(glm::vec2(forw.y, forw.z));
        glm::vec2 ryz = normalize(glm::vec2(ray.y, ray.z));

        print_vec3(forw);
        print_vec3(ray);
        print_vec2(fyz);
        print_vec2(ryz);
        float d = acos(dot(fyz, ryz));
        cout << "angle. = " << d << endl;
        m_rotation.x = d;
        cout << "New Rot. = " << m_rotation.x << endl;*/

        float dz = pos.z - target.z;
        float dy = pos.y - target.y;
        cout << "dz = " << dz << endl;
        cout << "dy = " << dy << endl;
        cout << "angle = " << degrees(atan(dy/dz))  << endl;
        m_rotation.x = degrees(atan(dy/dz));

        float dx = pos.x - target.x;
        m_rotation.y = -degrees(atan(dx/dz));
        cout << "dz = " << dz << endl;
        cout << "dx = " << dx << endl;
        cout << "angle = " << -degrees(atan(dx/dz))  << endl;

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
        setAccelerationVector(getForwardDirection());
    }

    void SetMovement(DIRECTION::ENUM direction, bool boolean) {
        if (boolean)
            setAccelerationVector(getForwardDirection());
        m_moving[direction] = boolean;
    }

private:
    static const size_t m_moving_size = 4;
    bool m_moving[m_moving_size];
    vec2 m_rotation;
    mat4 m_matrix;
    float m_movement_factor = 0.4f;

    glm::vec3 getForwardDirection() {
        return normalize(vec3(-cos(radians(m_rotation.x)) * sin(radians(m_rotation.y)),
                                                sin(radians(m_rotation.x)),
                                                cos(radians(m_rotation.x)) * cos(radians(m_rotation.y))));

    }

    glm::vec3 getLeftDirection() {
        return vec3(cos(radians(m_rotation.y)), 0, sin(radians(m_rotation.y)));
    }

    glm::vec3 getUpVector(){
        return normalize(cross(getForwardDirection(), getLeftDirection()));
    }
};

