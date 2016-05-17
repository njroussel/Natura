#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "icg_helper.h"
#include "../physics/material_point.h"
#include <glm/gtc/type_ptr.hpp>
#include <cstdint>

typedef enum DIRECTION {
    Forward = 0, Backward = 1
};


class Camera : public MaterialPoint{

public:

    Camera(vec3 &starting_position, vec2 &starting_rotation) : MaterialPoint(1.0, starting_position){
        m_rotation = vec2(starting_rotation.x, starting_rotation.y);
        m_matrix = IDENTITY_MATRIX;

        allowBackwardsSpeed(false);
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

    void AddRotation(vec2 &rotation) {
        m_rotation += rotation;
        if (isMoving()){
            if (isSlowingDown()){
                cout << "Slowing down" << endl;
                if (m_direction == Forward)
                    forceDirection(-getForwardDirection());
                else
                    forceDirection(getForwardDirection());
            }
            else{
                if (m_direction == Forward)
                    forceDirection(getForwardDirection());
                else
                    forceDirection(-getForwardDirection());
            }
        }
    }

    void SetMovement(DIRECTION direction, bool boolean) {
            m_direction = direction;
        if (boolean) {
            if (direction == DIRECTION::Forward)
                setAccelerationVector(getForwardDirection());
            else
                setAccelerationVector(-getForwardDirection());
        }
        else {
            if (m_direction == DIRECTION::Forward)
                setAccelerationVector(-getForwardDirection());
            else
                setAccelerationVector(getForwardDirection());
        }
    }

private:
    float m_movement_factor = 4.f;
    vec2 m_rotation;
    mat4 m_matrix;
    DIRECTION m_direction;

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

