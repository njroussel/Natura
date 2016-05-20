#pragma once

#include <vector>
#include "../../../../../../../../../../usr/include/c++/4.9/bits/stl_bvector.h"
#include "../../../external/glm/detail/type_vec.hpp"
#include "../../../../../../../../../../usr/include/c++/4.9/bits/stl_vector.h"

class BezierCurve {
public:
    BezierCurve(std::vector<glm::vec3> control_points){
        m_control_points = control_points;
    }

    glm::vec3 getPosition(float time){
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
        const float step = 0.01;
        const GLuint vert_count = static_cast<GLuint> (1.f / step);
        GLfloat curve_vertices[vert_count*3];

        for (int t = 0 ; t < vert_count*3 ; t += 3){
            float time = t * step / 3.f;
            glm::vec3 pos = getPosition(time) / TERRAIN_SCALE;
            curve_vertices[t] = pos.x;
            curve_vertices[t+1] = pos.y;
            curve_vertices[t+2] = pos.z;
        }

        GLuint program_id_ = icg_helper::LoadShaders("bezier_vshader.glsl",
                                              "bezier_fshader.glsl");
        if(!program_id_) {
            exit(EXIT_FAILURE);
        }

        glUseProgram(program_id_);
        GLuint vert_array_id;
        glGenVertexArrays(1, &vert_array_id);
        glBindVertexArray(vert_array_id);
        GLuint buffer_id;
        glGenBuffers(1, &buffer_id);
        glBindBuffer(GL_ARRAY_BUFFER, buffer_id);

        glBufferData(GL_ARRAY_BUFFER, sizeof(curve_vertices), curve_vertices, GL_STATIC_DRAW);

        GLint posAttrib = glGetAttribLocation(program_id_, "position");
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindVertexArray(0);
        glUseProgram(0);


        // Draw
        glUseProgram(program_id_);

        glm::mat4 MVP = projection * view * model;
        GLint MVP_id = glGetUniformLocation(program_id_, "MVP");
        glUniformMatrix4fv(MVP_id, 1, GL_FALSE, value_ptr(MVP));

        glBindVertexArray(vert_array_id);
        glDrawArrays(GL_LINE_STRIP, 0, vert_count);
        glBindVertexArray(0);

        glUseProgram(0);

        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &buffer_id);
        glDeleteVertexArrays(1, &vert_array_id);
    }

private:
    std::vector<glm::vec3> m_control_points;

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