#pragma once

#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>
#include <cstdint>

class Grid {

private:
    GLuint vertex_array_id_;                // vertex array object
    GLuint vertex_buffer_object_position_;  // memory buffer for positions
    GLuint vertex_buffer_object_index_;     // memory buffer for indices
    GLuint program_id_;                     // GLSL shader program ID
    GLuint texture_id_;                     // texture ID
    GLuint texture_id_2_;                   // texture ID 2
    GLuint num_indices_;                    // number of vertices to render
    GLuint M_id_;                           // model matrix ID
    GLuint V_id_;                           // view matrix ID
    GLuint P_id_;                           // proj matrix ID
    uint32_t mSideNbPoints;                 // grids side X nb of vertices;
    bool mCleanedUp;                        // check if the grid is cleaned before its destruction.

public:

    Grid(uint32_t sideSize) {
        mSideNbPoints = sideSize;
        mCleanedUp = true;
    }

    ~Grid() {
        if (!mCleanedUp)
            Cleanup();
    }

    void Cleanup() {
        mCleanedUp = true;
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &vertex_buffer_object_position_);
        glDeleteBuffers(1, &vertex_buffer_object_index_);
        glDeleteVertexArrays(1, &vertex_array_id_);
        glDeleteProgram(program_id_);
        glDeleteTextures(1, &texture_id_);
    }

    void Init(GLuint texture_) {

        mCleanedUp = false; // Until the next Cleanup() call ...

        // compile the shaders.
        program_id_ = icg_helper::LoadShaders("grid_vshader.glsl",
                                              "grid_fshader.glsl");
        if (!program_id_) {
            exit(EXIT_FAILURE);
        }

        glUseProgram(program_id_);

        // vertex one vertex array
        glGenVertexArrays(1, &vertex_array_id_);
        glBindVertexArray(vertex_array_id_);

        // vertex coordinates and indices
        {
            std::vector<GLfloat> vertices;
            std::vector<GLuint> indices;

            float sideX = 1 / float(mSideNbPoints);

            for (int i = 0; i < mSideNbPoints; i++) {
                for (int j = 0; j < mSideNbPoints; j++) {
                    vertices.push_back(i * sideX);
                    vertices.push_back(j * sideX);
                }
            }

            for (unsigned int j = 0; j < mSideNbPoints - 1; j++) {
                if (j % 2 == 0) {
                    for (int i = 0; i < mSideNbPoints; i++) {
                        indices.push_back(mSideNbPoints * j + i);
                        indices.push_back(mSideNbPoints * j + i + mSideNbPoints);
                    }
                } else {
                    for (int i = mSideNbPoints - 1; i >= 0; i--) {
                        indices.push_back(mSideNbPoints * j + i);
                        indices.push_back(mSideNbPoints * j + i + mSideNbPoints);
                    }
                }
            }

            num_indices_ = indices.size();

            // position buffer
            glGenBuffers(1, &vertex_buffer_object_position_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_position_);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
                         &vertices[0], GL_STATIC_DRAW);

            // vertex indices
            glGenBuffers(1, &vertex_buffer_object_index_);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_index_);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
                         &indices[0], GL_STATIC_DRAW);

            // position shader attribute
            GLuint loc_position = glGetAttribLocation(program_id_, "position");
            glEnableVertexAttribArray(loc_position);
            glVertexAttribPointer(loc_position, 2, GL_FLOAT, DONT_NORMALIZE,
                                  ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }

        //declaring uniforms
        {
            glm::vec3 La = glm::vec3(1.0f, 1.0f, 1.0f);
            glm::vec3 Ld = glm::vec3(1.0f, 1.0f, 1.0f);
            glm::vec3 light_pos = glm::vec3(100.0f, 100.0f, 100.0f);

            GLuint La_id = glGetUniformLocation(program_id_, "La");
            GLuint Ld_id = glGetUniformLocation(program_id_, "Ld");
            GLuint light_pos_id = glGetUniformLocation(program_id_, "light_pos");

            glUniform3fv(La_id, ONE, glm::value_ptr(La));
            glUniform3fv(Ld_id, ONE, glm::value_ptr(Ld));
            glUniform3fv(light_pos_id, ONE, glm::value_ptr(light_pos));

            glm::vec3 ka = glm::vec3(0.5f, 0.5f, 0.5f);
            glm::vec3 kd = glm::vec3(0.5f, 0.5f, 0.5f);

            GLuint ka_id = glGetUniformLocation(program_id_, "ka");
            GLuint kd_id = glGetUniformLocation(program_id_, "kd");
            GLuint alpha_id = glGetUniformLocation(program_id_, "alpha");

            glUniform3fv(ka_id, ONE, glm::value_ptr(ka));
            glUniform3fv(kd_id, ONE, glm::value_ptr(kd));



            // other uniforms
            M_id_ = glGetUniformLocation(program_id_, "model");
            V_id_ = glGetUniformLocation(program_id_, "view");
            P_id_ = glGetUniformLocation(program_id_, "projection");
        }

        //binding textures
        {
            //perlin texture
            this->texture_id_ = texture_;
            glBindTexture(GL_TEXTURE_2D, texture_id_);
            glUniform1i(glGetUniformLocation(program_id_, "perlin_tex"),
                        0 /*GL_TEXTURE0*/);

            //temporary color texture
            const int ColormapSize = 2;
            //GLfloat tex[3 * ColormapSize] = {0.0, 0.2, 0.45, 158.0f / 255.0f, 181.0f / 255.0f, 210.0f / 255.0f};
            GLfloat tex[3 * ColormapSize] = {0.0, 0.2, 0.45, 158.0f / 255.0f, 181.0f / 255.0f, 210.0f / 255.0f};
            glGenTextures(1, &texture_id_2_);
            glBindTexture(GL_TEXTURE_1D, texture_id_2_);
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, ColormapSize, 0, GL_RGB, GL_FLOAT, tex);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            GLuint tex_id = glGetUniformLocation(program_id_, "colormap");
            glUniform1i(tex_id, 1 /*GL_TEXTURE0*/);
            // check_error_gl();

        }
        // to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }


    void Draw(float amplitude, float time, const glm::mat4 &model = IDENTITY_MATRIX,
              const glm::mat4 &view = IDENTITY_MATRIX,
              const glm::mat4 &projection = IDENTITY_MATRIX) {
        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

        glUniform1f(glGetUniformLocation(program_id_, "amplitude"), amplitude);

        glUniformMatrix4fv(M_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(model));
        glUniformMatrix4fv(V_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(view));
        glUniformMatrix4fv(P_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(projection));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id_);


        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_1D, texture_id_2_);

        // draw
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLE_STRIP, num_indices_, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        glUseProgram(0);
    }
};
