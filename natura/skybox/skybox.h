#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "icg_helper.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

static const unsigned int NbCubeVertices = 36;
static const glm::vec3 CubeVertices[] =
{
    glm::vec3(-0.5, -0.5, -0.5),
    glm::vec3(-0.5, 0.5, -0.5),
    glm::vec3(0.5, -0.5, -0.5),
    glm::vec3(-0.5, 0.5, -0.5),
    glm::vec3(0.5, -0.5, -0.5),
    glm::vec3(0.5, 0.5, -0.5),
    glm::vec3(0.5, 0.5, 0.5),
    glm::vec3(0.5, -0.5, 0.5),
    glm::vec3(0.5, 0.5, -0.5),
    glm::vec3(0.5, -0.5, 0.5),
    glm::vec3(0.5, 0.5, -0.5),
    glm::vec3(0.5, -0.5, -0.5),
    glm::vec3(0.5, 0.5, 0.5),
    glm::vec3(-0.5, 0.5, 0.5),
    glm::vec3(0.5, -0.5, 0.5),
    glm::vec3(-0.5, 0.5, 0.5),
    glm::vec3(0.5, -0.5, 0.5),
    glm::vec3(-0.5, -0.5, 0.5),
    glm::vec3(-0.5, -0.5, 0.5),
    glm::vec3(-0.5, -0.5, -0.5),
    glm::vec3(0.5, -0.5, 0.5),
    glm::vec3(-0.5, -0.5, -0.5),
    glm::vec3(0.5, -0.5, 0.5),
    glm::vec3(0.5, -0.5, -0.5),
    glm::vec3(-0.5, 0.5, -0.5),
    glm::vec3(-0.5, -0.5, -0.5),
    glm::vec3(-0.5, 0.5, 0.5),
    glm::vec3(-0.5, -0.5, -0.5),
    glm::vec3(-0.5, 0.5, 0.5),
    glm::vec3(-0.5, -0.5, 0.5),
    glm::vec3(0.5, 0.5, -0.5),
    glm::vec3(-0.5, 0.5, -0.5),
    glm::vec3(0.5, 0.5, 0.5),
    glm::vec3(-0.5, 0.5, -0.5),
    glm::vec3(0.5, 0.5, 0.5),
    glm::vec3(-0.5, 0.5, 0.5)
};

static const unsigned int NbCubeUVs = 36;

static const glm::vec2 CubeUVs[] =
{
    glm::vec2(0.333, 0.75),
    glm::vec2(0.666, 0.75),
    glm::vec2(0.333, 0.5),
    glm::vec2(0.666, 0.75),
    glm::vec2(0.333, 0.5),
    glm::vec2(0.666, 0.5),

    glm::vec2(0.666, 0.25),
    glm::vec2(0.333, 0.25),
    glm::vec2(0.666, 0.5),
    glm::vec2(0.333, 0.25),
    glm::vec2(0.666, 0.5),
    glm::vec2(0.333, 0.5),

    glm::vec2(0.666, 0.25),
    glm::vec2(0.666, 0.0),
    glm::vec2(0.333, 0.25),
    glm::vec2(0.666, 0.0),
    glm::vec2(0.333, 0.25),
    glm::vec2(0.333, 0.0),

    glm::vec2(0.0, 0.75),
    glm::vec2(0.333, 0.75),
    glm::vec2(0.0, 0.5),
    glm::vec2(0.333, 0.75),
    glm::vec2(0.0, 0.5),
    glm::vec2(0.333, 0.5),

    glm::vec2(0.666, 0.75),
    glm::vec2(0.333, 0.75),
    glm::vec2(0.666, 1.0),
    glm::vec2(0.333, 0.75),
    glm::vec2(0.666, 1.0),
    glm::vec2(0.333, 1.0),

    glm::vec2(0.666, 0.5),
    glm::vec2(0.666, 0.75),
    glm::vec2(1.0, 0.5),
    glm::vec2(0.666, 0.75),
    glm::vec2(1.0, 0.5),
    glm::vec2(1.0, 0.75)
};

class SkyBox {

    private:
        GLuint vertex_array_id_;        // vertex array object
        GLuint program_id_;             // GLSL shader program ID
        GLuint vertex_buffer_object_;   // memory buffer
        GLuint texture_id_;             // texture ID
        GLuint texture_id_cube;             // texture ID
        glm::mat4 model_matrix_;        // model matrix

    public:
        void Init() {
            // compile the shaders.
            program_id_ = icg_helper::LoadShaders("cube_vshader.glsl",
                                                  "cube_fshader.glsl");
            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id_);

            // vertex one vertex array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            // vertex coordinates
            {
                // buffer
                glGenBuffers(1, &vertex_buffer_object_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
                glBufferData(GL_ARRAY_BUFFER, NbCubeVertices * sizeof(glm::vec3),
                             &CubeVertices[0], GL_STATIC_DRAW);

                // attribute
                GLuint vertex_point_id = glGetAttribLocation(program_id_, "vpoint");
                glEnableVertexAttribArray(vertex_point_id);
                glVertexAttribPointer(vertex_point_id, 3, GL_FLOAT, DONT_NORMALIZE,
                                      ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            }

            // texture coordinates
            {
                // buffer
                glGenBuffers(1, &vertex_buffer_object_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
                glBufferData(GL_ARRAY_BUFFER, NbCubeUVs * sizeof(glm::vec2),
                             &CubeUVs[0], GL_STATIC_DRAW);

                // attribute
                GLuint vertex_texture_coord_id = glGetAttribLocation(program_id_,
                                                                     "vtexcoord");
                glEnableVertexAttribArray(vertex_texture_coord_id);
                glVertexAttribPointer(vertex_texture_coord_id, 2, GL_FLOAT,
                                      DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            }

            {
                glGenTextures (1, &texture_id_cube);
                glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_cube);

                // format cube map texture
                glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                // load each image and copy into a side of the cube-map texture
                assert (
                        load_cube_map_side (texture_id_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, "front.tga"));
                assert (
                        load_cube_map_side (texture_id_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, "back.tga"));
                assert (
                        load_cube_map_side (texture_id_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, "top.tga"));
                assert (
                        load_cube_map_side (texture_id_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, "bottom.tga"));
                assert (
                        load_cube_map_side (texture_id_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, "right.tga"));
                assert (
                        load_cube_map_side (texture_id_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_X, "left.tga"));

                GLuint tex_id = glGetUniformLocation(program_id_, "tex_cube");
                glUniform1i(tex_id, 1 /*GL_TEXTURE0*/);

                // cleanup
                glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            }

            // create the model matrix
            model_matrix_ = IDENTITY_MATRIX;
            model_matrix_ = glm::translate(model_matrix_,
                                           glm::vec3(0.0f, 0.0f, 0.6f));
        }

    bool load_cube_map_side (GLuint texture, GLenum side_target, const char* file_name) {

        int x, y, n;
        int force_channels = 4;
        unsigned char* image = stbi_load(file_name,
                                         &x, &y, &n, 0);

        if (!image) {
            fprintf (stderr, "ERROR: could not load %s\n", file_name);
            return false;
        }


        if(n == 3) {
            glTexImage2D(side_target, 0, GL_RGB, x, y, 0,
                         GL_RGB, GL_UNSIGNED_BYTE, image);
        } else if(n == 4) {
            glTexImage2D(side_target, 0, GL_RGBA, x, y, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, image);
        }

        free (image);
        return true;
    }

        void Cleanup() {
            glBindVertexArray(0);
            glUseProgram(0);
            glDeleteBuffers(1, &vertex_buffer_object_);
            glDeleteProgram(program_id_);
            glDeleteVertexArrays(1, &vertex_array_id_);
            glDeleteTextures(1, &texture_id_);
        }

        void Draw(const glm::mat4& view_projection){
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // bind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_id_);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_cube);


            // setup MVP
            mat4 model = scale(model_matrix_, vec3(100.0f));
            glm::mat4 MVP = view_projection * model;
            GLuint MVP_id = glGetUniformLocation(program_id_, "MVP");
            glUniformMatrix4fv(MVP_id, 1, GL_FALSE, value_ptr(MVP));

            GLuint M_id = glGetUniformLocation(program_id_, "M");
            glUniformMatrix4fv(M_id, 1, GL_FALSE, value_ptr(model));

            // draw
            glDrawArrays(GL_TRIANGLES,0, NbCubeVertices);

            glBindVertexArray(0);
            glUseProgram(0);
        }
};
