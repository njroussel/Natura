#pragma once

#include "icg_helper.h"
#include "glm/gtc/type_ptr.hpp"

class PerlinQuad {

private:
    GLuint vertex_array_id_;        // vertex array object
    GLuint program_id_;             // GLSL shader program ID
    GLuint vertex_buffer_object_;   // memory buffer
    GLuint texture_id_;             // texture ID

    int p_[256] = {173, 78, 203, 128, 97, 146, 63, 65, 159, 43, 212, 48, 34, 171, 183, 197,
    170, 69, 103, 216, 167, 208, 189, 93, 228, 49, 226, 59, 96, 156, 1, 72, 182, 188, 83, 166, 179, 143,
    47, 64, 193, 139, 28, 120, 231, 245, 52, 66, 21, 39, 90, 217, 151, 6, 45, 180, 210, 253, 87, 18,
    141, 114, 94, 177, 104, 8, 20, 181, 154, 205, 250, 227, 42, 106, 98, 117, 144, 15, 37, 229, 142, 35,
    25, 190, 248, 100, 62, 85, 194, 145, 110, 195, 137, 32, 56, 255, 74, 201, 80, 61, 24, 81, 38, 235,
    111, 162, 160, 23, 132, 134, 200, 73, 31, 118, 244, 163, 91, 135, 19, 169, 246, 175, 55, 149, 101,
    207, 233, 238, 199, 16, 153, 147, 254, 75, 220, 79, 185, 165, 99, 206, 124, 4, 60, 186, 22, 17, 148,
    202, 236, 196, 158, 51, 10, 127, 76, 176, 92, 232, 12, 213, 107, 27, 84, 164, 123, 178, 109, 204,
    133, 108, 41, 222, 2, 105, 57, 77, 209, 67, 218, 122, 225, 211, 150, 14, 242, 168, 121, 136, 224,
    9, 102, 116, 157, 112, 33, 125, 7, 130, 161, 50, 30, 29, 140, 70, 86, 0, 219, 172, 129, 58, 252, 184,
    221, 46, 240, 192, 113, 187, 11, 71, 251, 89, 249, 131, 234, 198, 138, 54, 44, 95, 13, 247, 5, 243,
    152, 26, 68, 237, 82, 53, 115, 40, 215, 191, 223, 3, 241, 239, 36, 126, 214, 119, 230, 174, 88, 155};

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

    void Draw(const glm::mat4 &MVP, float H, float frequency, float lacunarity, float offset, int octaves, glm::vec2 displ) {
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
        glUniform2fv(glGetUniformLocation(program_id_, "displacement"), ONE, glm::value_ptr(displ));
        glUniform1iv(glGetUniformLocation(program_id_, "p"), 256, p_);
        // draw
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindVertexArray(0);
        glUseProgram(0);
    }
};
