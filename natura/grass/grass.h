#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "icg_helper.h"
#include "../terrain/terrain.h"
#include <glm/gtc/type_ptr.hpp>
#include <cstdint>

class Grass {

private :

    GLuint program_id_;
    GLuint vertex_array_id_;   // memory buffer
    GLuint vertex_buffer_object_;   // memory buffer
    GLuint m_texture_id;
    GLuint m_grass_triangles_count;
    float m_fGrassPatchOffsetMin;
    float m_fGrassPatchOffsetMax;
    float m_fGrassPatchHeight;
    float m_minXpos;
    float m_maxXpos;
    float m_minZpos;
    float m_maxZpos;
    Terrain *m_terrain;

public :

    Grass(float fGrassPatchOffsetMin, float fGrassPatchOffsetMax, float fGrassPatchHeight, float minXpos, float maxXpos,
          float minZpos, float maxZpos, Terrain *terrain) {
        m_fGrassPatchOffsetMin = fGrassPatchOffsetMin;
        m_fGrassPatchOffsetMax = fGrassPatchOffsetMax;
        m_fGrassPatchHeight = fGrassPatchHeight;
        m_minXpos = minXpos;
        m_maxXpos = maxXpos;
        m_minZpos = minZpos;
        m_maxZpos = maxZpos;
        m_terrain = terrain;
    }

    void Init() {

        program_id_ = icg_helper::LoadShaders("grass_vshader.glsl",
                                              "grass_fshader.glsl",
                                              "grass_gshader.glsl");
        glUseProgram(program_id_);

        // vertex one vertex Array
        glGenVertexArrays(1, &vertex_array_id_);
        glBindVertexArray(vertex_array_id_);

        // vertex coordinates
        {
            vector<GLfloat> vertex_point;

            m_grass_triangles_count = 0;
            glm::vec3 vCurPatchPos(m_minXpos, 0.0f, m_minZpos);


            while (vCurPatchPos.x < m_maxXpos) {
                vCurPatchPos.z = m_minZpos;
                while (vCurPatchPos.z < m_maxZpos) {
                    vCurPatchPos.z += m_fGrassPatchOffsetMin; //+
                    //(m_fGrassPatchOffsetMax - m_fGrassPatchOffsetMin) * rand() / float(RAND_MAX);
                    vCurPatchPos.y = m_terrain->getHeight(vec2(vCurPatchPos.x, vCurPatchPos.z));
                    m_grass_triangles_count += 1;
                    vertex_point.push_back(vCurPatchPos.x);
                    vertex_point.push_back(vCurPatchPos.y);
                    vertex_point.push_back(vCurPatchPos.z);
                }

                vCurPatchPos.x += m_fGrassPatchOffsetMin;//+
                // (m_fGrassPatchOffsetMax - m_fGrassPatchOffsetMin) * rand() / float(RAND_MAX);
            }

            // buffer
            glGenBuffers(1, &vertex_buffer_object_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
            glBufferData(GL_ARRAY_BUFFER, vertex_point.size() * sizeof(GL_FLOAT),
                         vertex_point.data(), GL_STATIC_DRAW);

            // attribute
            GLuint vertex_point_id = glGetAttribLocation(program_id_, "vpoint");
            glEnableVertexAttribArray(vertex_point_id);
            glVertexAttribPointer(vertex_point_id, 3, GL_FLOAT, DONT_NORMALIZE,
                                  ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }


        m_texture_id = loadDDS("grassPack.dds");
        glUniform1i(glGetUniformLocation(program_id_, "gSampler"), 0 /*GL_TEXTURE*/);


        glBindVertexArray(0);
        glUseProgram(0);
    }

    void Draw(float time, const glm::mat4 &model = IDENTITY_MATRIX,
              const glm::mat4 &view = IDENTITY_MATRIX,
              const glm::mat4 &projection = IDENTITY_MATRIX) {

        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

        glUniformMatrix4fv(glGetUniformLocation(program_id_, "model"), ONE, DONT_TRANSPOSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(program_id_, "view"), ONE, DONT_TRANSPOSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(program_id_, "projection"), ONE, DONT_TRANSPOSE,
                           glm::value_ptr(projection));

        glUniform1f(glGetUniformLocation(program_id_, "time"), time);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture_id);


        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // draw
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_POINTS, 0, m_grass_triangles_count);
        glDisable(GL_BLEND);

        glBindVertexArray(0);
        glUseProgram(0);
    }

    GLuint loadDDS(const char *imagepath) {
        unsigned char header[124];

        FILE *fp;

        /* try to open the file */
        fp = fopen(imagepath, "rb");
        if (fp == NULL) {
            return 0;
        }

        /* verify the type of file */
        char filecode[4];
        fread(filecode, 1, 4, fp);
        if (strncmp(filecode, "DDS ", 4) != 0) {
            fclose(fp);
            return 0;
        }

        /* get the surface desc */
        fread(&header, 124, 1, fp);

        unsigned int height = *(unsigned int *) &(header[8]);
        unsigned int width = *(unsigned int *) &(header[12]);
        unsigned int linearSize = *(unsigned int *) &(header[16]);
        unsigned int mipMapCount = *(unsigned int *) &(header[24]);
        unsigned int fourCC = *(unsigned int *) &(header[80]);

        unsigned char *buffer;
        unsigned int bufsize;

        /* how big is it going to be including all mipmaps? */
        bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
        buffer = (unsigned char *) malloc(bufsize * sizeof(unsigned char));
        fread(buffer, 1, bufsize, fp);

        /* close the file pointer */
        fclose(fp);

        unsigned int format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;

        // Create one OpenGL texture
        GLuint textureID;
        glGenTextures(1, &textureID);

        // "Bind" the newly created texture : all future texture functions will modify this texture
        glBindTexture(GL_TEXTURE_2D, textureID);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        unsigned int blockSize = 8;
        unsigned int offset = 0;

        /* load the mipmaps */
        for (unsigned int level = 0; level < 1 && (width || height); ++level) {
            unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
            glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
                                   0, size, buffer + offset);

            offset += size;
            width = width/2 > 1 ? width/2 : 1;
            height = height/2 > 1 ? height/2 : 1;
        }

        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glGenerateMipmap(GL_TEXTURE_2D);

        free(buffer);

        //Unbind the texture
        glBindTexture(GL_TEXTURE_2D, 0);


        return textureID;
    }


};


