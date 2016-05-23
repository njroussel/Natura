#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "icg_helper.h"
#include "../physics/material_point.h"
#include "../terrain/terrain.h"
#include <glm/gtc/type_ptr.hpp>
#include <cstdint>

typedef enum DIRECTION {
    Forward = 0, Backward = 1, Left = 2, Right = 3, Up = 4, Down = 5
};


class Camera : public MaterialPoint {
public:

    Camera(vec3 &starting_position, vec2 &starting_rotation, Terrain *terrain) : MaterialPoint(1.0, starting_position) {
        m_rotation = vec2(starting_rotation.x, starting_rotation.y);
        m_matrix = IDENTITY_MATRIX;
        m_pressed[Forward] = false;
        m_pressed[Backward] = false;
        m_pressed[Left] = false;
        m_pressed[Right] = false;
        m_pressed[Up] = false;
        m_pressed[Down] = false;
        m_terrain = terrain;
        m_fps_mode = false;
    }

    void CalculateMatrix() {
        m_matrix = IDENTITY_MATRIX;
        m_matrix = glm::rotate(m_matrix, radians(m_rotation.x), vec3(1.0f, 0.0f, 0.0f));
        m_matrix = glm::rotate(m_matrix, radians(m_rotation.y), vec3(0.0f, 1.0f, 0.0f));
        m_matrix = glm::translate(m_matrix, getPosition());
    }

    void tick() {
        AddRotation();
        _update_acc();
        CalculateMatrix();
        MaterialPoint::tick();
        //cout << " pos ===== = " << m_position.x << " " << m_position.y << " " << m_position.z << endl;
        if (m_fps_mode){
            /* We snap the camera to the ground */
            axis___ = true;
            float h = -1 * TERRAIN_SCALE * m_terrain->getHeight(glm::vec2(-m_position.x/TERRAIN_SCALE, -m_position.z/TERRAIN_SCALE)) - 0.2f;
            axis___ = false;
            m_position.y = h;
        }
    }

    mat4 &GetMatrix() {
        return m_matrix;
    }

    mat4 getMirroredMatrix(){
        mat4 mirrored = IDENTITY_MATRIX;
        mirrored = glm::rotate(mirrored, radians(-m_rotation.x), vec3(1.0f, 0.0f, 0.0f));
        mirrored = glm::rotate(mirrored, radians(m_rotation.y), vec3(0.0f, 1.0f, 0.0f));
        vec3 pos = vec3(m_position.x, -m_position.y, m_position.z);
        mirrored = glm::translate(mirrored, pos);
        return mirrored;
    }

    void AddRotation() {
        vec2 addRotation = vec2(0.0f, 0.0f);
        if (m_pressed[Left]) {
            glm::vec2 rot = vec2(0, -1);
            addRotation += rot;
        }
        if (m_pressed[Right]) {
            glm::vec2 rot = vec2(0, 1);
            addRotation += rot;
        }
        if (m_pressed[Up]) {
            m_tmp = true;
            glm::vec2 rot = vec2(-1, 0);
            addRotation += rot;
        }
        if (m_pressed[Down]) {
            glm::vec2 rot = vec2(1, 0);
            addRotation += rot;
        }

        m_rotation += addRotation;

        if (isMoving() && length(addRotation) != 0 && !m_fps_mode) {
            forceDirection(getForwardDirection());
        }
    }

    bool m_tmp = false;

    bool hasAcceleration(DIRECTION dir) {
        return m_pressed[dir];
    }

    void setMovement(DIRECTION dir) {
        m_pressed[dir] = true;
    }

    void stopMovement(DIRECTION dir) {
        m_pressed[dir] = false;
    }

    void enableFPSMode(bool enable){
        m_fps_mode = enable;
    }

private:
    glm::vec2 m_rotation;
    glm::mat4 m_matrix;
    bool m_pressed[4];
    Terrain *m_terrain;
    bool m_fps_mode;

    glm::vec3 getForwardDirection() {
        vec3 tmp = (vec3(-cos(radians(m_rotation.x)) * sin(radians(m_rotation.y)),
                              -sin(radians(m_rotation.x + 180.0f)),
                              cos(radians(m_rotation.x)) * cos(radians(m_rotation.y))));
        /*cout << tmp.x <<endl;
        cout << tmp.y <<endl;
        cout << tmp.z<<endl;
        cout << " " << endl;*/
        return normalize(tmp);
    }

    void _update_acc() {
        vec3 fwdDirection = getForwardDirection();
        if (m_pressed[Forward] && !m_pressed[Backward]) {
            setAccelerationVector(fwdDirection);
        }
        else if (!m_pressed[Forward] && m_pressed[Backward]) {
            setAccelerationVector(-fwdDirection);
        }
        else {
            if (isMoving()) {
                vec3 currentSpeed = getSpeedVector();
                float direction = dot(currentSpeed, fwdDirection);
                if (direction < 0) {
                    setAccelerationVector(fwdDirection);
                }
                else {
                    setAccelerationVector(-fwdDirection);
                }
            }
        }
    }
};
