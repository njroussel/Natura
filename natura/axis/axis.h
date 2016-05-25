#pragma once
#include "icg_helper.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/rotate_vector.hpp>

GLfloat lineVertices[] = {
        0.0f,  0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f,  0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f,  0.0f, 0.0f,
        0.0f, 0.0f, 1.0f
};

class Axis {

private:
    GLuint program_id_;
    GLuint lineVAO, lineVBO;

public:
    void Init() {
        program_id_ = icg_helper::LoadShaders("axis_vshader.glsl",
                                              "axis_fshader.glsl");
        if(!program_id_) {
            exit(EXIT_FAILURE);
        }

        glUseProgram(program_id_);

        glGenVertexArrays(1, &lineVAO);
        glBindVertexArray(lineVAO);

        glGenBuffers(1, &lineVBO);
        glBindBuffer(GL_ARRAY_BUFFER, lineVBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);

        GLint posAttrib = glGetAttribLocation(program_id_, "position");
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindVertexArray(0);
        glUseProgram(0);
    }

    void Cleanup() {
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &lineVBO);
        glDeleteVertexArrays(1, &lineVAO);
    }

    void Draw(const glm::mat4& model = IDENTITY_MATRIX,
              const glm::mat4& view = IDENTITY_MATRIX,
              const glm::mat4& projection = IDENTITY_MATRIX){
        glUseProgram(program_id_);

        glm::mat4 MVP = projection * view;
        GLint MVP_id = glGetUniformLocation(program_id_, "MVP");
        glUniformMatrix4fv(MVP_id, 1, GL_FALSE, value_ptr(MVP));

        glBindVertexArray(lineVAO);
        glDrawArrays(GL_LINES, 0, 6);
        glBindVertexArray(0);

        glUseProgram(0);
    }
};