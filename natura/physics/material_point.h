#pragma once

#include "../../external/glm/detail/type_vec.hpp"
#include "../../external/glm/detail/type_vec3.hpp"
#include <limits>

class MaterialPoint {
public:

    MaterialPoint(float mass, glm::vec3 initial_pos = glm::vec3(0, 0, 0)) : m_acceleration(0, 0, 0), m_speed(0, 0, 0) {
        m_speed = vec3(0.0f);
        m_position = initial_pos;
        m_mass = mass;
        m_max_speed = 0.4f;
    }

    glm::vec3 getPosition() {
        return m_position;
    }

    void setSpeed(float max_speed) {
        if (max_speed > 0)
            m_max_speed = max_speed;
    }

    void setAccelerationVector(glm::vec3 acc) {
        float curr_speed = length(m_speed);
        m_acceleration = normalize(acc);
        float theta = dot(m_acceleration, m_speed) / curr_speed;
        if (theta < 0) {
            m_speed = glm::vec3(-m_acceleration.x * curr_speed,
                                -m_acceleration.y * curr_speed,
                                -m_acceleration.z * curr_speed);
        }
        else {
            m_speed = glm::vec3(m_acceleration.x * curr_speed,
                                m_acceleration.y * curr_speed,
                                m_acceleration.z * curr_speed);
        }
    }

    void forceDirection(glm::vec3 acc) {
        float curr_speed = length(m_speed);
        float curr_acc = length(m_acceleration);
        float theta = dot(normalize(acc), normalize(m_acceleration));

        if (theta < 0) {
            m_acceleration = -acc * curr_acc;
        }
        else {
            m_acceleration = acc * curr_acc;
        }

        float alpha = dot(normalize(m_speed), normalize(m_acceleration));
        if (alpha < 0) {
            m_speed = glm::vec3(-m_acceleration.x * curr_speed,
                                -m_acceleration.y * curr_speed,
                                -m_acceleration.z * curr_speed);
        }
        else {
            m_speed = glm::vec3(m_acceleration.x * curr_speed,
                                m_acceleration.y * curr_speed,
                                m_acceleration.z * curr_speed);
        }
    }

    virtual void tick() {
        _update_pos();
    }

    glm::vec3 getAccelerationVector() {
        return m_acceleration;
    }

    glm::vec3 getSpeedVector() {
        return m_speed;
    }

    bool isSlowingDown() {
        float d = dot(normalize(m_speed), normalize(m_acceleration));
        return d <= 0;
    }

    bool isMoving() {
        return length(m_speed) > 0.0f;
    }


    void freeze() {
        m_speed = glm::vec3(0, 0, 0);
    }

private:
    float m_mass;
    float m_max_speed;
    glm::vec3 m_acceleration;
    glm::vec3 m_speed;
    glm::vec3 m_position;

    void _update_pos() {
        m_speed += 0.01f * glm::vec3(m_acceleration.x,
                                     m_acceleration.y,
                                     m_acceleration.z);

        float curr_speed = length(m_speed);

        float epsilon = 0.02f;
        if (curr_speed > m_max_speed) {
            m_speed = m_speed * m_max_speed / curr_speed;
        }
        if (curr_speed < epsilon && isSlowingDown()) {
            m_speed = glm::vec3(0, 0, 0);
            m_acceleration = glm::vec3(0, 0, 0);
        }

        m_position += glm::vec3(m_speed.x,
                                m_speed.z,
                                m_speed.y);
    }
};