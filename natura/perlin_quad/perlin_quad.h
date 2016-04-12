#pragma once

#include "icg_helper.h"
#include "glm/gtc/type_ptr.hpp"

class PerlinQuad {

private:
    GLuint vertex_array_id_;        // vertex array object
    GLuint program_id_;             // GLSL shader program ID
    GLuint vertex_buffer_object_;   // memory buffer
    GLuint texture_id_;             // texture ID

public:

    void Init() {
        // compile the shaders
        program_id_ = icg_helper::LoadShaders("perlin_quad_vshader.glsl",
                                              "perlin_quad_fshader.glsl");
        if (!program_id_) {
            exit(EXIT_FAILURE);
        }

        glUseProgram(program_id_);

        // vertex one vertex Array
        glGenVertexArrays(1, &vertex_array_id_);
        glBindVertexArray(vertex_array_id_);

        // vertex coordinates
        {
            const GLfloat vertex_point[] = { /*V1*/ -1.0f, -1.0f, 0.0f,
                    /*V2*/ +1.0f, -1.0f, 0.0f,
                    /*V3*/ -1.0f, +1.0f, 0.0f,
                    /*V4*/ +1.0f, +1.0f, 0.0f};
            // buffer
            glGenBuffers(1, &vertex_buffer_object_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_point),
                         vertex_point, GL_STATIC_DRAW);

            // attribute
            GLuint vertex_point_id = glGetAttribLocation(program_id_, "vpoint");
            glEnableVertexAttribArray(vertex_point_id);
            glVertexAttribPointer(vertex_point_id, 3, GL_FLOAT, DONT_NORMALIZE,
                                  ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }

        // to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void Cleanup() {
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &vertex_buffer_object_);
        glDeleteProgram(program_id_);
        glDeleteVertexArrays(1, &vertex_array_id_);
        glDeleteTextures(1, &texture_id_);
    }

    void Draw(const glm::mat4 &MVP, float H, float frequency, float lacunarity, float offset, int octaves) {
        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);


        // setup MVP
        GLuint MVP_id = glGetUniformLocation(program_id_, "MVP");
        glUniformMatrix4fv(MVP_id, 1, GL_FALSE, value_ptr(MVP));

        glUniform1f(glGetUniformLocation(program_id_, "H"), H);
        glUniform1f(glGetUniformLocation(program_id_, "lacunarity"), lacunarity);
        glUniform1f(glGetUniformLocation(program_id_, "frequency"), frequency);
        glUniform1f(glGetUniformLocation(program_id_, "offset"), offset);
        glUniform1i(glGetUniformLocation(program_id_, "octaves"), octaves);

        // draw
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindVertexArray(0);
        glUseProgram(0);
    }
};
