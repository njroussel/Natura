#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "icg_helper.h"
#include "../physics/material_point.h"
#include "../terrain/terrain.h"
#include "bezier/bezier_curve.h"
#include <glm/gtc/type_ptr.hpp>
#include <cstdint>

typedef enum DIRECTION {
    Forward = 0, Backward = 1, Left = 2, Right = 3, Up = 4, Down = 5
};

typedef enum CAMERA_MODE {
    Flythrough = 0, Fps = 1, Bezier = 2
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
        m_mode = CAMERA_MODE::Flythrough;
        m_pos_curve = NULL;
        m_look_curve = NULL;
    }

    void CalculateMatrix() {
        m_matrix = IDENTITY_MATRIX;
        m_matrix = glm::rotate(m_matrix, radians(m_rotation.y), vec3(1.0f, 0.0f, 0.0f));
        m_matrix = glm::rotate(m_matrix, radians(m_rotation.x), vec3(0.0f, 1.0f, 0.0f));
        m_matrix = glm::translate(m_matrix, getPosition());
    }

    void tick() {
        if (m_mode != CAMERA_MODE::Bezier){
            AddRotation();
            _update_acc();
        }
        CalculateMatrix();
        if (m_mode != CAMERA_MODE::Bezier)
            MaterialPoint::tick();
        if (m_mode == CAMERA_MODE::Fps){
            /* We snap the camera to the ground */
            float h = -1 * TERRAIN_SCALE * m_terrain->getHeight(glm::vec2(-m_position.x/TERRAIN_SCALE, -m_position.z/TERRAIN_SCALE)) - 0.2f;
            m_position.y = h;
        }
        else if (m_mode == CAMERA_MODE::Bezier) {
            double time = glfwGetTime();
            double delta_time = time - m_bezier_time;
            glm::vec3 look_point = -m_look_curve->getPosition(delta_time);
            glm::vec3 pos_point = -m_pos_curve->getPosition(delta_time);
            m_position = pos_point * TERRAIN_SCALE;
            lookAtPoint(glm::vec3(TERRAIN_SCALE * look_point.x, TERRAIN_SCALE * look_point.y, TERRAIN_SCALE * look_point.z));
        }
    }

    mat4 &GetMatrix() {
        return m_matrix;
    }

    mat4 getMirroredMatrix(float axisHeight) {
        mat4 mirrored = IDENTITY_MATRIX;
        vec3 pos = getPosition();
        mirrored = glm::rotate(mirrored, radians(-m_rotation.y), vec3(1.0f, 0.0f, 0.0f));
        mirrored = glm::rotate(mirrored, radians(m_rotation.x), vec3(0.0f, 1.0f, 0.0f));
        cout << pos.y << endl;
        cout << axisHeight << endl;
        vec3 new_pos = vec3(pos.x, (axisHeight - (pos.y - axisHeight)), pos.z);
        cout << new_pos.y << endl;
        mirrored = glm::translate(mirrored, new_pos);
        return mirrored;
    }

    void lookAtPoint(vec3 point) {
        vec3 position = getPosition();
        if(position == point){
            return;
        }

        vec3 newFwd = normalize(point - position);
        vec3 up = vec3(0.0f, 1.0f, 0.0f);
        vec3 right = vec3(0.0f, 0.0f, 1.0f);

        float newYRotation = degrees(acos(dot(newFwd, up)));

        vec3 fwdPlaneDirection = normalize(vec3(newFwd.x, 0, newFwd.z));
        float newXRotation =  degrees(acos(dot(fwdPlaneDirection, right)));

        m_rotation.y = -(newYRotation - 90);
        m_rotation.x =  position.x < point.x ? -newXRotation : newXRotation;
    }

    void AddRotation() {
        vec2 addRotation = vec2(0.0f, 0.0f);
        float speed = m_mode == CAMERA_MODE::Fps ? 1.f : m_rotation_speed;
        if (m_pressed[Left]) {
            glm::vec2 rot = vec2(-speed, 0);
            addRotation += rot;
        }
        if (m_pressed[Right]) {
            glm::vec2 rot = vec2(speed, 0);
            addRotation += rot;
        }
        if (m_pressed[Up]) {
            glm::vec2 rot = vec2(0, -speed);
            addRotation += rot;
        }
        if (m_pressed[Down]) {
            glm::vec2 rot = vec2(0, speed);
            addRotation += rot;
        }

        m_rotation += addRotation;
        if (m_mode == CAMERA_MODE::Fps){
            if (m_rotation.y > 90) m_rotation.y = 90;
            else if (m_rotation.y < -90) m_rotation.y = -90;
        }
    }

    bool hasAcceleration(DIRECTION dir) {
        return m_pressed[dir];
    }

    void setMovement(DIRECTION dir) {
        m_pressed[dir] = true;
    }

    void stopMovement(DIRECTION dir) {
        m_pressed[dir] = false;
    }

    void enableFpsMode(){
        m_mode = CAMERA_MODE::Fps;
    }

    void enableFlyThroughtMode(){
        m_mode = CAMERA_MODE::Flythrough;
    }

    void enableBezierMode(BezierCurve *pos_curve, BezierCurve *look_curve){
        m_mode = CAMERA_MODE::Bezier;
        m_pos_curve = pos_curve;
        m_look_curve = look_curve;
        m_bezier_time = glfwGetTime();
    }

    CAMERA_MODE getCameraMode() {
        return m_mode;
    }

    void AddRotationFPS(glm::vec2 rot) {
        if (m_mode == CAMERA_MODE::Fps)
            m_rotation += rot;
    }

    void setPosition(glm::vec3 pos) {
        m_position = pos;
    }

    glm::vec3 getFrontPoint(float dist = 1.f) {
        return m_position +  getForwardDirection();
    }

private:
    glm::vec2 m_rotation;
    glm::mat4 m_matrix;
    bool m_pressed[6];
    BezierCurve *m_look_curve;
    BezierCurve *m_pos_curve;
    double m_bezier_time;
    float m_rotation_speed = 2.f;

    Terrain *m_terrain;
    CAMERA_MODE m_mode;

    glm::vec3 getForwardDirection() {
        vec3 tmp = vec3(-sin(radians(m_rotation.y + 90.0f)) * sin(radians(m_rotation.x)),

                        -cos(radians(m_rotation.y + 90.0f)),
                        sin(radians(m_rotation.y + 90.0f)) * cos(radians(m_rotation.x)));
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
