#pragma once

#include <vector>
#include "../../../external/glm/detail/type_vec.hpp"

class BezierCurve {
public:
    glm::vec3 getPosition(float time){
        time = glm::mod(time / m_time_length, 1.f);
        size_t n = m_control_points.size()-1;
        glm::vec3 res = glm::vec3(0, 0, 0);
        for (size_t j = 0 ; j <= n ; j ++){
            res += m_control_points[j]*B(time, n, j);
        }
        return res;
    }

    void addPoint(glm::vec3 point){
        m_control_points.push_back(point);
        if (m_init_done){
            CleanUp();
            Init();
        }
        else {
            Init();
        }
    }

    void enableLoop(bool enable){
        size_t ctrl_points_count = m_control_points.size();
        if (ctrl_points_count > 2) {
            if (enable){
                if (m_control_points[0] != m_control_points[ctrl_points_count-1])
                    addPoint(m_control_points[0]);
            }
            else{
                if (m_control_points[0] == m_control_points[ctrl_points_count-1]){
                    m_control_points.pop_back();
                    if (m_init_done){
                        CleanUp();
                        Init();
                    }
                }
            }
        }
    }

    void Clear(){
        m_control_points.clear();
    }

    size_t Size() {
        return m_control_points.size();
    }

    void setTimeLength(float time){
        m_time_length = time;
    }

    void Init() {
        if (m_control_points.size() < 2)
            return;
        m_init_done = true;
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

    void Draw(const glm::mat4& model = IDENTITY_MATRIX,
              const glm::mat4& view = IDENTITY_MATRIX,
              const glm::mat4& projection = IDENTITY_MATRIX){
        if (!m_init_done)
            return;
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

    void CleanUp(){
        glDeleteBuffers(1, &m_buffer_id);
        glDeleteVertexArrays(1, &m_ver_array_id);
    }

private:
    std::vector<glm::vec3> m_control_points;
    GLuint m_program_id;
    GLuint m_ver_array_id;
    GLuint m_vert_count;
    GLuint m_buffer_id;
    float m_time_length = 1.f;
    bool m_init_done = false;

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