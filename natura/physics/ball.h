#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "icg_helper.h"
#include "../physics/material_point.h"
#include "../misc/observer_subject/messages/ball_out_of_bound_message.h"

class Ball : public MaterialPoint, public Subject{

public:
    Ball(glm::vec3 starting_position, glm::vec3 starting_vector, Terrain *terrain) : MaterialPoint(2.3f, starting_position) {
        m_terrain = terrain;
        m_speed = 0.4f * starting_vector;
        m_frozen = false;
        this->setAccelerationVector(glm::vec3(0, -1.0f, 0));

        string error;
        // obj files can contains material informations
        vector<tinyobj::material_t> materials;
        string filename = "sphere.obj";

        bool objLoadReturn = tinyobj::LoadObj(m_shapes, materials, error, filename.c_str());

        if (!error.empty()) {
            cerr << error << endl;
        }

        if (!objLoadReturn) {
            exit(EXIT_FAILURE);
        }

        // only load the first shape from the obj file
        // (see tinyobjloader for multiple shapes inside one .obj file)

        int number_of_vertices = m_shapes[0].mesh.positions.size();
        int number_of_indices = m_shapes[0].mesh.indices.size();
        int number_of_normals = m_shapes[0].mesh.normals.size();
        printf("Loaded mesh '%s' (#V=%d, #I=%d, #N=%d)\n", filename.c_str(),
               number_of_vertices, number_of_indices, number_of_normals);

        // vertex one vertex Array
        glGenVertexArrays(1, &m_vertex_array_id);
        glBindVertexArray(m_vertex_array_id);

        // vertex buffer
        glGenBuffers(ONE, &m_vertex_buffer_object);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer_object);
        glBufferData(GL_ARRAY_BUFFER, number_of_vertices * sizeof(float),
                     &m_shapes[0].mesh.positions[0], GL_STATIC_DRAW);

        // normal buffer
        glGenBuffers(ONE, &m_vertex_normal_buffer_object);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_normal_buffer_object);
        glBufferData(GL_ARRAY_BUFFER, number_of_normals * sizeof(float),
                     &m_shapes[0].mesh.normals[0], GL_STATIC_DRAW);

        // index buffer
        GLuint vertex_buffer_object_indices;
        glGenBuffers(ONE, &vertex_buffer_object_indices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_indices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     number_of_indices * sizeof(unsigned int),
                     &m_shapes[0].mesh.indices[0], GL_STATIC_DRAW);

        glBindVertexArray(0);

        m_program_id = icg_helper::LoadShaders("ball_vshader.glsl",
                                               "ball_fshader.glsl");

        glUseProgram(m_program_id);

        glm::vec3 ka = glm::vec3(0.5f, 0.1f, 0.1f);
        glm::vec3 kd = glm::vec3(0.9f, 0.5f, 0.5f);
        glm::vec3 ks = glm::vec3(0.8f, 0.8f, 0.8f);
        float alpha = 60.0f;

        GLuint ka_id = glGetUniformLocation(m_program_id, "ka");
        GLuint kd_id = glGetUniformLocation(m_program_id, "kd");
        GLuint ks_id = glGetUniformLocation(m_program_id, "ks");
        GLuint alpha_id = glGetUniformLocation(m_program_id, "alpha");

        glUniform3fv(ka_id, ONE, glm::value_ptr(ka));
        glUniform3fv(kd_id, ONE, glm::value_ptr(kd));
        glUniform3fv(ks_id, ONE, glm::value_ptr(ks));
        glUniform1f(alpha_id, alpha);


        glm::vec3 La = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 Ld = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 Ls = glm::vec3(1.0f, 1.0f, 1.0f);

        GLuint La_id = glGetUniformLocation(m_program_id, "La");
        GLuint Ld_id = glGetUniformLocation(m_program_id, "Ld");
        GLuint Ls_id = glGetUniformLocation(m_program_id, "Ls");

        glUniform3fv(La_id, ONE, glm::value_ptr(La));
        glUniform3fv(Ld_id, ONE, glm::value_ptr(Ld));
        glUniform3fv(Ls_id, ONE, glm::value_ptr(Ls));
    }

    void tick(glm::vec3 referencePoint) {
        if (length(m_speed) < 0.01f || m_frozen) {
            if (!m_frozen){
                m_frozen = true;
                m_froze_time = glfwGetTime();
            }
            else{
                double time = glfwGetTime();
                if (time - m_froze_time > BALL_MAX_FROZEN_TIME){
                    notify(new BallOutOfBoundsMessage(this));
                }
            }
        }
        else{
            _update_pos();
            try {
                float terrainHeight = m_terrain->getHeight(glm::vec2(m_position.x, m_position.z));
                if (m_position.y < terrainHeight) {
                    float epsilon = 0.005f;
                    float zDiffXaxis = m_terrain->getHeight(glm::vec2(m_position.x + epsilon, m_position.z)) -
                                       m_terrain->getHeight(glm::vec2(m_position.x - epsilon, m_position.z));

                    float zDiffYaxis = m_terrain->getHeight(glm::vec2(m_position.x, m_position.z + epsilon)) -
                                       m_terrain->getHeight(glm::vec2(m_position.x, m_position.z - epsilon));

                    glm::vec3 normal = -glm::normalize(
                            glm::cross(glm::vec3(2 * epsilon, zDiffXaxis, 0.0f), glm::vec3(0.0, zDiffYaxis, 2 * epsilon)));
                    glm::vec3 m = m_speed - dot(m_speed, normal) * normal;
                    glm::vec3 endpoint = m_position + m + m;
                    glm::vec3 new_speed = endpoint - (m_position + m_speed);
                    new_speed.y = -m_speed.y;
                    float curr_speed = length(m_speed);
                    if (curr_speed < 0.01f) {
                        m_speed = glm::vec3(0.0f);
                    }
                    else {
                        m_position = glm::vec3(m_position.x, terrainHeight, m_position.z);
                        m_speed = normalize(new_speed) * curr_speed * 0.7f;
                    }
                }
            }
            catch (std::runtime_error e){
                /* Need to destroy the ball. */
                notify(new BallOutOfBoundsMessage(this));
            }
        }
    }

    void CleanUp() {
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &m_vertex_buffer_object);
        glDeleteBuffers(1, &m_vertex_normal_buffer_object);
        glDeleteVertexArrays(1, &m_vertex_array_id);
    }

    void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
              const glm::mat4 &view = IDENTITY_MATRIX,
              const glm::mat4 &projection = IDENTITY_MATRIX) {

        glm::mat4 M = model;
        M = glm::translate(model, m_position);
        M = glm::scale(M, glm::vec3(0.1f));

        glUseProgram(m_program_id);
        glBindVertexArray(m_vertex_array_id);

        GLint vertex_point_id = glGetAttribLocation(m_program_id, "vpoint");
        if (vertex_point_id >= 0) {
            glEnableVertexAttribArray(vertex_point_id);

            glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer_object);
            glVertexAttribPointer(vertex_point_id, 3 /*vec3*/, GL_FLOAT,
                                  DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }

        // vertex attribute id for normals
        GLint vertex_normal_id = glGetAttribLocation(m_program_id, "vnormal");
        if (vertex_normal_id >= 0) {
            glEnableVertexAttribArray(vertex_normal_id);
            glBindBuffer(GL_ARRAY_BUFFER, m_vertex_normal_buffer_object);
            glVertexAttribPointer(vertex_normal_id, 3 /*vec3*/, GL_FLOAT,
                                  DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }

        GLint model_id = glGetUniformLocation(m_program_id, "model");
        glUniformMatrix4fv(model_id, ONE, DONT_TRANSPOSE, glm::value_ptr(M));

        GLint view_id = glGetUniformLocation(m_program_id, "view");
        glUniformMatrix4fv(view_id, ONE, DONT_TRANSPOSE, glm::value_ptr(view));

        GLint projection_id = glGetUniformLocation(m_program_id, "projection");
        glUniformMatrix4fv(projection_id, ONE, DONT_TRANSPOSE, glm::value_ptr(projection));

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDrawElements(GL_TRIANGLES, /*#vertices*/ m_shapes[0].mesh.indices.size(),
                       GL_UNSIGNED_INT, ZERO_BUFFER_OFFSET);

        glDisable(GL_BLEND);

        vertex_point_id = glGetAttribLocation(m_program_id, "vpoint");
        if (vertex_point_id >= 0) {
            glDisableVertexAttribArray(vertex_point_id);
        }

        glUseProgram(0);
        glBindVertexArray(0);
    }

private:

    std::vector<tinyobj::shape_t> m_shapes;
    GLuint m_vertex_buffer_object;           // memory buffer
    GLuint m_vertex_normal_buffer_object;    // memory buffer
    GLuint m_vertex_array_id;                // vertex array object
    GLuint m_program_id;
    bool m_frozen;
    float m_froze_time;
    Terrain *m_terrain;
};

