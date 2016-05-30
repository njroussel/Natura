#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "icg_helper.h"
#include "../shadows/attrib_locations.h"
#include "../config.h"
#include <glm/gtc/type_ptr.hpp>
#include <cstdint>

class Grid {

private:
    GLuint vertex_array_id_;                // vertex array object
    GLuint vertex_buffer_object_position_;  // memory buffer for positions
    GLuint vertex_buffer_object_index_;     // memory buffer for indices
    GLuint program_id_;                     // GLSL shader program ID
    GLuint texture_perlin_id_;              // texture ID
    GLuint texture_left_id_;              // texture ID
    GLuint texture_low_id_;              // texture ID
    GLuint texture_low_left_id_;
    GLuint texture_grass_id_;               // texture ID
    GLuint texture_rock_id_;                // texture ID
    GLuint texture_snow_id_;                // texture ID
    GLuint texture_sand_id_;                // texture ID
    GLuint texture_deep_water_id_;                // texture ID
    GLuint num_indices_;                    // number of vertices to render
    GLuint M_id_;                           // model matrix ID
    GLuint V_id_;                           // view matrix ID
    GLuint P_id_;                           // proj matrix ID
    uint32_t mSideNbPoints;                 // grids side X nb of vertices;
    bool mCleanedUp;                        // check if the grid is cleaned before its destruction.
    glm::vec2 m_indices;                    // Tile indices on the terrain.
    GLuint m_shadow_pid;
    bool m_use_shadows = false;                     // true if we need to generate the Z-buffe
    GLuint m_depth_tex;

public:

    Grid(uint32_t sideSize, glm::vec2 indices) {
        mSideNbPoints = sideSize;
        mCleanedUp = true;
        m_indices = indices;
    }

    ~Grid() {
        if (!mCleanedUp)
            Cleanup();
    }

    void setShadowPID(GLuint pid){
        m_shadow_pid = pid;
        glUniform1i(glGetUniformLocation(m_shadow_pid, "perlin_tex"), 0 /*GL_TEXTURE0*/);
        glUniform1i(glGetUniformLocation(m_shadow_pid, "left_tex"), 6/*GL_TEXTURE0*/);
        glUniform1i(glGetUniformLocation(m_shadow_pid, "low_tex"), 7 /*GL_TEXTURE0*/);
        glUniform1i(glGetUniformLocation(m_shadow_pid, "low_left_tex"), 8 /*GL_TEXTURE0*/);
        glUniform1i(glGetUniformLocation(m_shadow_pid, "shadow_map"), 9 /*GL_TEXTURE0*/);
    }

    void setUseShadowPID(bool enable){
        m_use_shadows = enable;
    }

    void setDepthTex(GLuint tex) {
        m_depth_tex = tex;
    }

    void setTextureId(int id) {
        this->texture_perlin_id_ = id;
    }

    void setTextureLeft(GLuint id){
        texture_left_id_ = id;
    }

    void setTextureLow(GLuint id){
        texture_low_id_ = id;
    }

    void setTextureLowLeft(GLuint id){
        texture_low_left_id_ = id;
    }

    void Cleanup() {
        mCleanedUp = true;
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &vertex_buffer_object_position_);
        glDeleteBuffers(1, &vertex_buffer_object_index_);
        glDeleteVertexArrays(1, &vertex_array_id_);
        glDeleteProgram(program_id_);
        glDeleteTextures(1, &texture_perlin_id_);
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
            mSideNbPoints++;
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
            glBindAttribLocation(program_id_, ATTRIB_LOC_position, "position");
            glLinkProgram(program_id_);
            //GLuint loc_position = glGetAttribLocation(program_id_, "position");
            glEnableVertexAttribArray(ATTRIB_LOC_position);
            glVertexAttribPointer(ATTRIB_LOC_position, 2, GL_FLOAT, DONT_NORMALIZE,
                                  ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }

        //declaring uniforms
        {
            glm::vec3 La = glm::vec3(1.0f, 1.0f, 1.0f);
            glm::vec3 Ld = glm::vec3(1.0f, 1.0f, 1.0f);
            glm::vec3 Ls = glm::vec3(1.0f, 1.0f, 1.0f);
            glm::vec3 light_pos = glm::vec3(0.0f, 100.0f, 0.0f);

            GLuint La_id = glGetUniformLocation(program_id_, "La");
            GLuint Ld_id = glGetUniformLocation(program_id_, "Ld");
            GLuint Ls_id = glGetUniformLocation(program_id_, "Ls");
            GLuint light_pos_id = glGetUniformLocation(program_id_, "light_pos");

            glUniform3fv(La_id, ONE, glm::value_ptr(La));
            glUniform3fv(Ld_id, ONE, glm::value_ptr(Ld));
            glUniform3fv(Ls_id, ONE, glm::value_ptr(Ls));
            glUniform3fv(light_pos_id, ONE, glm::value_ptr(light_pos));

            glm::vec3 ka = glm::vec3(0.18f, 0.1f, 0.1f);
            glm::vec3 kd = glm::vec3(0.9f, 0.5f, 0.5f);
            glm::vec3 ks = glm::vec3(0.01f, 0.01f, 0.01f);
            float alpha = 60.0f;

            GLuint ka_id = glGetUniformLocation(program_id_, "ka");
            GLuint kd_id = glGetUniformLocation(program_id_, "kd");
            GLuint ks_id = glGetUniformLocation(program_id_, "ks");
            GLuint alpha_id = glGetUniformLocation(program_id_, "alpha");

            glUniform3fv(ka_id, ONE, glm::value_ptr(ka));
            glUniform3fv(kd_id, ONE, glm::value_ptr(kd));
            glUniform3fv(ks_id, ONE, glm::value_ptr(ks));
            glUniform1f(alpha_id, alpha);

            // other uniforms
            M_id_ = glGetUniformLocation(program_id_, "model");
            V_id_ = glGetUniformLocation(program_id_, "view");
            P_id_ = glGetUniformLocation(program_id_, "projection");
        }

        //binding perlin texture
        {
            //perlin texture
            this->texture_perlin_id_ = texture_;
            //glBindTexture(GL_TEXTURE_2D, texture_id_);
            glUniform1i(glGetUniformLocation(program_id_, "perlin_tex"), 0 /*GL_TEXTURE0*/);
            glUniform1i(glGetUniformLocation(program_id_, "left_tex"), 6/*GL_TEXTURE0*/);
            glUniform1i(glGetUniformLocation(program_id_, "low_tex"), 7 /*GL_TEXTURE0*/);
            glUniform1i(glGetUniformLocation(program_id_, "low_left_tex"), 8 /*GL_TEXTURE0*/);
            glUniform1i(glGetUniformLocation(program_id_, "shadow_map"), 9 /*GL_TEXTURE0*/);
        }

        loadTexture("grass.tga", &texture_grass_id_, 1, glGetUniformLocation(program_id_, "grass_tex"));
        loadTexture("rock.tga", &texture_rock_id_, 2, glGetUniformLocation(program_id_, "rock_tex"));
        loadTexture("snow.tga", &texture_snow_id_, 3, glGetUniformLocation(program_id_, "snow_tex"));
        loadTexture("sand.tga", &texture_sand_id_, 4, glGetUniformLocation(program_id_, "sand_tex"));
        loadTexture("water.tga", &texture_deep_water_id_, 5, glGetUniformLocation(program_id_, "water_tex"));

        // to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void Draw(glm::vec2 chunk_pos, glm::vec2 indices, float amplitude, float water_height, float time, const glm::mat4 &model = IDENTITY_MATRIX,
              const glm::mat4 &view = IDENTITY_MATRIX,
              const glm::mat4 &projection = IDENTITY_MATRIX) {
        GLuint pid = m_use_shadows ? m_shadow_pid : program_id_;
        glUseProgram(pid);
        glBindVertexArray(vertex_array_id_);
        //glUniform1i(glGetUniformLocation(program_id_, "shadow_map"), 1);
        glUniform1f(glGetUniformLocation(pid, "amplitude"), amplitude);

        glUniform1f(glGetUniformLocation(pid, "water_height"), water_height);

        glUniformMatrix4fv(glGetUniformLocation(pid, "model"), ONE, DONT_TRANSPOSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(pid, "view"), ONE, DONT_TRANSPOSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(pid, "projection"), ONE, DONT_TRANSPOSE, glm::value_ptr(projection));
        glUniform2fv(glGetUniformLocation(pid, "quad_indices"), ONE, glm::value_ptr(indices));
        glUniform2fv(glGetUniformLocation(pid, "chunk_pos"), ONE, glm::value_ptr(chunk_pos));
        cout << "chunkpos = " << chunk_pos.x << " " << chunk_pos.y << endl;
        glUniform1i(glGetUniformLocation(pid, "terrain_size"), TERRAIN_CHUNK_SIZE);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_perlin_id_);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_grass_id_);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture_rock_id_);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, texture_snow_id_);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, texture_sand_id_);

        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, texture_deep_water_id_);

        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, texture_left_id_);

        glUniform1i(glGetUniformLocation(pid, "left_present"), texture_left_id_ != 0);

        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, texture_low_id_);
        glUniform1i(glGetUniformLocation(pid, "low_present"), texture_low_id_ != 0);

        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, texture_low_left_id_);
        glUniform1i(glGetUniformLocation(pid, "low_left_present"), texture_low_left_id_ != 0);

        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, m_depth_tex);

        // draw
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLE_STRIP, num_indices_, GL_UNSIGNED_INT, 0);
        glDisable(GL_BLEND);

        glBindVertexArray(0);
        glUseProgram(0);
    }

    GLuint getPID(){
        return program_id_;
    }

    void loadTexture(string filename, GLuint *texture_id, int tex_index, GLint tex_id_uniform) {
        // load grass texture
        int width;
        int height;
        int nb_component;
        // set stb_image to have the same coordinates as OpenGL
        stbi_set_flip_vertically_on_load(1);
        unsigned char *image = stbi_load(filename.c_str(), &width,
                                         &height, &nb_component, 0);

        if (image == nullptr) {
            throw (string("Failed to load texture"));
        }

        glGenTextures(1, texture_id);
        glBindTexture(GL_TEXTURE_2D, *texture_id);

        if (nb_component == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                         GL_RGB, GL_UNSIGNED_BYTE, image);
        } else if (nb_component == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, image);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);

        glUniform1i(tex_id_uniform, tex_index /*GL_TEXTURE*/);

        // cleanup
        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

};

Grid *BASE_TILE;