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

    Camera(glm::vec3 &starting_position, glm::vec2 &starting_rotation, Terrain *terrain) : MaterialPoint(0.4f, starting_position) {
        m_rotation = glm::vec2(starting_rotation.x, starting_rotation.y);
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
        m_bezier_step = TICK;
        m_rotation_inertia = glm::vec3(0, 0, 0);
    }

    void CalculateMatrix() {
        m_matrix = IDENTITY_MATRIX;
        m_matrix = glm::rotate(m_matrix, glm::radians(m_rotation.y), glm::vec3(1.0f, 0.0f, 0.0f));
        m_matrix = glm::rotate(m_matrix, glm::radians(m_rotation.x), glm::vec3(0.0f, 1.0f, 0.0f));
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
            try {
                float h = -1 * TERRAIN_SCALE * m_terrain->getHeight(
                        glm::vec2(-m_position.x / TERRAIN_SCALE, -m_position.z / TERRAIN_SCALE)) - 0.2f;
                m_position.y = h;
            }
            catch (std::runtime_error e){
                /* Fall back mode : FlyThrough */
                m_mode = CAMERA_MODE::Flythrough;
            }
        }
        else if (m_mode == CAMERA_MODE::Bezier) {
            m_bezier_time += m_bezier_step;
            glm::vec3 look_point = -m_look_curve->getPosition(m_bezier_time);
            glm::vec3 pos_point = -m_pos_curve->getPosition(m_bezier_time);
            m_position = pos_point * TERRAIN_SCALE;
            lookAtPoint(glm::vec3(TERRAIN_SCALE * look_point.x, TERRAIN_SCALE * look_point.y, TERRAIN_SCALE * look_point.z));
        }
    }

    glm::mat4 &GetMatrix() {
        return m_matrix;
    }

    glm::mat4 getMirroredMatrix(float axisHeight) {
        glm::mat4 mirrored = IDENTITY_MATRIX;
        glm::vec3 pos = getPosition();
        mirrored = glm::rotate(mirrored, glm::radians(-m_rotation.y), glm::vec3(1.0f, 0.0f, 0.0f));
        mirrored = glm::rotate(mirrored, glm::radians(m_rotation.x), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::vec3 new_pos = glm::vec3(pos.x, (axisHeight - (pos.y - axisHeight)), pos.z);
        mirrored = glm::translate(mirrored, new_pos);
        return mirrored;
    }

    void lookAtPoint(glm::vec3 point) {
        glm::vec3 position = getPosition();
        if(position == point){
            return;
        }

        glm::vec3 newFwd = normalize(point - position);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 right = glm::vec3(0.0f, 0.0f, 1.0f);

        float newYRotation = glm::degrees(acos(dot(newFwd, up)));

        glm::vec3 fwdPlaneDirection = glm::normalize(glm::vec3(newFwd.x, 0, newFwd.z));
        float newXRotation =  glm::degrees(acos(dot(fwdPlaneDirection, right)));

        m_rotation.y = -(newYRotation - 90);
        m_rotation.x =  position.x < point.x ? -newXRotation : newXRotation;
    }

    void AddRotation() {
        glm::vec2 addRotation = glm::vec2(0.0f, 0.0f);
        float speed = m_mode == CAMERA_MODE::Fps ? 1.f : m_rotation_speed;
        bool key_pressed = m_pressed[Left] || m_pressed[Right] || m_pressed[Up] || m_pressed[Down];
        if (!key_pressed && (m_rotation_inertia.x != 0 || m_rotation_inertia.y != 0)){
            if (m_rotation_inertia.x < 0) m_rotation_inertia.x += m_rotation_inertia_factor;
            else if (m_rotation_inertia.x > 0) m_rotation_inertia.x -= m_rotation_inertia_factor;
            if (m_rotation_inertia.y < 0) m_rotation_inertia.y += m_rotation_inertia_factor;
            else if (m_rotation_inertia.y > 0) m_rotation_inertia.y -= m_rotation_inertia_factor;
        }
        else {
            if (m_pressed[Left]) {
                m_rotation_inertia.x -= m_rotation_inertia_factor;
            }
            if (m_pressed[Right]) {
                m_rotation_inertia.x += m_rotation_inertia_factor;
            }
            if (m_pressed[Up]) {
                m_rotation_inertia.y -= m_rotation_inertia_factor;
            }
            if (m_pressed[Down]) {
                m_rotation_inertia.y += m_rotation_inertia_factor;
            }
        }
        if (m_rotation_inertia.x > 1.f) m_rotation_inertia.x = 1.f;
        else if (m_rotation_inertia.x < -1.f) m_rotation_inertia.x = -1.f;
        if (m_rotation_inertia.y > 1.f) m_rotation_inertia.y = 1.f;
        else if (m_rotation_inertia.y < -1.f) m_rotation_inertia.y = -1.f;
        glm::vec2 rot = glm::vec2(speed*m_rotation_inertia.x, speed*m_rotation_inertia.y);
        addRotation += rot;

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
        m_bezier_time = 0.f;
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

    float getBezierStep() {
        return m_bezier_step;
    }

    void setBezierStep(float step) {
        m_bezier_step = step;
        if (m_bezier_step < m_bezier_step_threshold)
            m_bezier_step = m_bezier_step_threshold;
    }

private:
    glm::vec2 m_rotation;
    glm::mat4 m_matrix;
    bool m_pressed[6];
    BezierCurve *m_look_curve;
    BezierCurve *m_pos_curve;
    double m_bezier_time;
    float const m_rotation_speed = 4.f;
    float m_bezier_step;
    const float m_bezier_step_threshold = 0.001f;
    glm::vec3 m_rotation_inertia;
    const float m_rotation_inertia_factor = 0.125f;

    Terrain *m_terrain;
    CAMERA_MODE m_mode;

    glm::vec3 getForwardDirection() {
        float rot_y = m_rotation.y;
        if (m_mode == CAMERA_MODE::Fps){
            /* We need to do this in order to be able to walk when looking up. */
            rot_y = 0.0;
        }
        glm::vec3 tmp = glm::vec3(-sin(glm::radians(rot_y + 90.0f)) * sin(glm::radians(m_rotation.x)),
                        -cos(glm::radians(rot_y + 90.0f)),
                        sin(glm::radians(rot_y + 90.0f)) * cos(glm::radians(m_rotation.x)));
        return normalize(tmp);
    }

    void _update_acc() {
        glm::vec3 fwdDirection = getForwardDirection();
        if (m_pressed[Forward] && !m_pressed[Backward]) {
            forceSpeedDirectionAlongAcceleration(fwdDirection);
        }
        else if (!m_pressed[Forward] && m_pressed[Backward]) {
            forceSpeedDirectionAlongAcceleration(-fwdDirection);
        }
        else {
            if (isMoving()) {
                glm::vec3 currentSpeed = getSpeedVector();
                float direction = dot(currentSpeed, fwdDirection);
                if (direction < 0) {
                    forceSpeedDirectionAlongAcceleration(fwdDirection);
                }
                else {
                    forceSpeedDirectionAlongAcceleration(-fwdDirection);
                }
            }
        }
    }
};
