#pragma once

#include "../../external/glm/detail/type_vec.hpp"
#include "../../external/glm/detail/type_vec3.hpp"
#include <limits>

class MaterialPoint {
public:
    MaterialPoint(float mass, glm::vec3 initial_pos = glm::vec3(0,0,0)) : m_acceleration(0,0,0), m_speed(0,0,0) {
        m_position = initial_pos;
        m_mass = mass;
        m_max_speed = std::numeric_limits<float>::max();
    }

    glm::vec3 getPosition(){
        _update_pos();
        return m_position;
    }

    void capSpeed(float max_speed){
        if (max_speed > 0)
            m_max_speed = max_speed;
    }

    void setAccelerationVector(glm::vec3 acc){
        //float curr_speed = sqrt(dot(m_speed, m_speed));
        //m_speed = glm::vec3(0,0,0);
        m_acceleration = acc;
        /*m_speed = glm::vec3(   m_acceleration.x * curr_speed,
                                m_acceleration.z * curr_speed,
                                m_acceleration.y * curr_speed);*/
        _update_pos();
    }

    void forceDirection(glm::vec3 acc) {
        float curr_speed = sqrt(dot(m_speed, m_speed));
        float curr_acc = sqrt(dot(m_acceleration, m_acceleration));
        float theta = dot(normalize(acc), normalize(m_acceleration));
        cout << "theta  = " << theta << endl;
        if (theta < 0){
            m_acceleration = -acc;
        }
        else{
            m_acceleration = acc;
        }
        float alpha = dot(normalize(m_speed), normalize(m_acceleration));
        if (alpha < 0){
            m_speed = glm::vec3(   -m_acceleration.x * curr_speed,
                                -m_acceleration.z * curr_speed,
                                -m_acceleration.y * curr_speed);
        }
        else {
            m_speed = glm::vec3(   m_acceleration.x * curr_speed,
                                    m_acceleration.z * curr_speed,
                                    m_acceleration.y * curr_speed);
        }

        //_update_pos();
    }

    glm::vec3 getAccelerationVector() {
        //_update_pos();
        return m_acceleration;
    }

    glm::vec3 getSpeedVector() {
        //_update_pos();
        return m_speed;
    }

    bool isSlowingDown() {
        //_update_pos();
        float d = dot(normalize(m_speed), normalize(m_acceleration));
        cout << "d = " << d << endl;
        return d < 0;
    }

    bool isMoving(){
        float s = dot(m_speed, m_speed);
        return s > 0.f;
    }

    void allowBackwardsSpeed(bool enable) {
        m_backward_speed = enable;
    }

    void freeze(){
        m_speed = glm::vec3(0, 0, 0);
    }

private:
    float m_mass;
    float m_max_speed;
    double m_last_refresh; // in seconds
    bool m_backward_speed;
    glm::vec3 m_acceleration;
    glm::vec3 m_speed;
    glm::vec3 m_position;

    void _update_pos(){
        double new_time = glfwGetTime();
        double elapsed_time = new_time - m_last_refresh;
        m_last_refresh = new_time;

        m_speed += glm::vec3(   m_acceleration.x * elapsed_time,
                                m_acceleration.z * elapsed_time,
                                m_acceleration.y * elapsed_time   );
        float curr_speed = sqrt(dot(m_speed, m_speed));
        cout << "speed = " << curr_speed << endl;
        if (curr_speed > m_max_speed){
            m_speed = normalize(m_speed);
            m_speed.x *= m_max_speed;
            m_speed.y *= m_max_speed;
            m_speed.z *= m_max_speed;
        }
        float epsilon = 0.1;
        if (!m_backward_speed && (curr_speed < epsilon && isSlowingDown())){
            m_speed = glm::vec3(0, 0, 0);
            m_acceleration = glm::vec3(0, 0, 0);
        }
        m_position += glm::vec3(   m_speed.x * elapsed_time,
                                   m_speed.z * elapsed_time,
                                   m_speed.y * elapsed_time   );
    }
};