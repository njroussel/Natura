#pragma once

#include <vector>
#include "../../../../../../../../../../usr/include/c++/4.9/bits/stl_bvector.h"
#include "../../../external/glm/detail/type_vec.hpp"
#include "../../../../../../../../../../usr/include/c++/4.9/bits/stl_vector.h"

class BezierCurve {
public:
    BezierCurve(std::vector<glm::vec3> control_points, float time_length){
        m_control_points = control_points;
        m_time_length = time_length;
    }

    void Init() {
        const float step = 0.1;
        m_vert_count = static_cast<GLuint> (m_time_length / step);
        GLfloat curve_vertices[m_vert_count*3];

        for (int t = 0 ; t < m_vert_count*3 ; t += 3){
            float time = t * step / 3.f;
            glm::vec3 pos = getPosition(time) / TERRAIN_SCALE;
            curve_vertices[t] = pos.x;
            curve_vertices[t+1] = pos.y;
            curve_vertices[t+2] = pos.z;
        }

        m_program_id = icg_helper::LoadShaders("bezier_vshader.glsl",
                                                     "bezier_fshader.glsl");
        if(!m_program_id) {
            exit(EXIT_FAILURE);
        }

        glUseProgram(m_program_id);
        glGenVertexArrays(1, &m_ver_array_id);
        glBindVertexArray(m_ver_array_id);
        glGenBuffers(1, &m_buffer_id);
        glBindBuffer(GL_ARRAY_BUFFER, m_buffer_id);

        glBufferData(GL_ARRAY_BUFFER, sizeof(curve_vertices), curve_vertices, GL_STATIC_DRAW);

        GLint posAttrib = glGetAttribLocation(m_program_id, "position");
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindVertexArray(0);
        glUseProgram(0);
    }

    glm::vec3 getPosition(float time){
        time = mod(time / m_time_length, 1.f);
        cout << "time = " << time << endl;
        size_t n = m_control_points.size()-1;
        glm::vec3 res = glm::vec3(0, 0, 0);
        for (size_t j = 0 ; j <= n ; j ++){
            res += m_control_points[j]*B(time, n, j);
        }
        return res;
    }

    void Draw(const glm::mat4& model = IDENTITY_MATRIX,
              const glm::mat4& view = IDENTITY_MATRIX,
              const glm::mat4& projection = IDENTITY_MATRIX){
        // Draw
        glUseProgram(m_program_id);

        glm::mat4 MVP = projection * view * model;
        GLint MVP_id = glGetUniformLocation(m_program_id, "MVP");
        glUniformMatrix4fv(MVP_id, 1, GL_FALSE, value_ptr(MVP));

        glBindVertexArray(m_ver_array_id);
        glDrawArrays(GL_LINE_STRIP, 0, m_vert_count);
        glBindVertexArray(0);

        glUseProgram(0);

        glBindVertexArray(0);
        glUseProgram(0);
    }

    void Cleanup(){
        glDeleteBuffers(1, &m_buffer_id);
        glDeleteVertexArrays(1, &m_ver_array_id);
    }

private:
    std::vector<glm::vec3> m_control_points;
    GLuint m_program_id;
    GLuint m_ver_array_id;
    GLuint m_vert_count;
    GLuint m_buffer_id;
    float m_time_length;

    float B(float t, int n, int i){
        if (i == 0 && n == 0)
            return 1.f;
        else if (!(0 <= i && i <= n))
            return 0.f;
        else {
            return (1.f - t) * B(t, n-1, i) + t * B(t, n-1, i-1);
        }
    }
};