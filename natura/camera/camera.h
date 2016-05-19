#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "icg_helper.h"
#include "../physics/material_point.h"
#include <glm/gtc/type_ptr.hpp>
#include <cstdint>

typedef enum DIRECTION {
    Forward = 0, Backward = 1, Left = 2, Right = 3
};


class Camera : public MaterialPoint {
public:
    Camera(vec3 &starting_position, vec2 &starting_rotation) : MaterialPoint(1.0, starting_position){
        m_rotation = vec2(starting_rotation.x, starting_rotation.y);
        m_matrix = IDENTITY_MATRIX;
        m_pressed[Forward] = false;
        m_pressed[Backward] = false;

        allowBackwardsSpeed(false);
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

    void AddRotation(vec2 &rotation) {
        m_rotation += rotation;
        if (m_rotation.x < -85.f)
            m_rotation.x = -85.f;
        if (m_rotation.x > 85)
            m_rotation.x = 85.f;
        if (isMoving()){
            forceDirection(getForwardDirection());
        }
        _update_acc();
    }

    void setAcceleration(DIRECTION dir){
        if (dir == Left || dir == Right)
            return;
        m_pressed[dir] = true;
        _update_acc();
    }

    void stopAcceleration(DIRECTION dir){
        if (dir == Left || dir == Right)
            return;
        m_pressed[dir] = false;
        _update_acc();
    }

    void snapToHeight(float h){
        m_position.z = h;
    }

private:
    glm::vec2 m_rotation;
    glm::mat4 m_matrix;
    bool m_pressed[2];

    glm::vec3 getForwardDirection() {
        return normalize(vec3(-cos(radians(m_rotation.x)) * sin(radians(m_rotation.y)),
                              sin(radians(m_rotation.x)),
                              cos(radians(m_rotation.x)) * cos(radians(m_rotation.y))));

    }

    void _update_acc(){
        if (m_pressed[Forward] && m_pressed[Backward]){
            setAccelerationVector(glm::vec3(0, 0, 0));
        }
        else if (m_pressed[Forward]) {
            setAccelerationVector(getForwardDirection());
        }
        else if (m_pressed[Backward]) {
            setAccelerationVector(-getForwardDirection());
        }
        else {
            cout << "All false" << endl;
            if (isMoving()){
                cout << "   Moving " << endl;
                glm::vec3 speed = getSpeedVector();
                glm::vec3 acc = getAccelerationVector();
                float theta = dot(speed, acc);
                cout << "   Theta = " << theta << endl;
                if (theta > 0){
                    setAccelerationVector(-acc);
                }
            }
        }
    }

};


/*class Camera : public MaterialPoint{

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
*/


