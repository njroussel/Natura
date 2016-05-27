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
                                              "grass_gshader.glsl",
                                              "grass_fshader.glsl");
        glUseProgram(program_id_);

        // vertex one vertex Array
        glGenVertexArrays(1, &vertex_array_id_);
        glBindVertexArray(vertex_array_id_);

        // vertex coordinates
        {
            vector<GLfloat> vertex_point;


            glm::vec3 vCurPatchPos(m_minXpos, 0.0f, m_minZpos);


            while (vCurPatchPos.x < m_maxXpos) {
                vCurPatchPos.z = m_minZpos;
                while (vCurPatchPos.z < m_maxZpos) {
                    vCurPatchPos.z += m_fGrassPatchOffsetMin +
                                      (m_fGrassPatchOffsetMax - m_fGrassPatchOffsetMin) * rand() / float(RAND_MAX);
                    vCurPatchPos.y = m_terrain->getHeight(vec2(vCurPatchPos.x, vCurPatchPos.z));

                    vertex_point.push_back(vCurPatchPos.x);
                    vertex_point.push_back(vCurPatchPos.y);
                    vertex_point.push_back(vCurPatchPos.z);
                }

                vCurPatchPos.x += m_fGrassPatchOffsetMin +
                                  (m_fGrassPatchOffsetMax - m_fGrassPatchOffsetMin) * rand() / float(RAND_MAX);
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

        glBindVertexArray(0);
        glUseProgram(0);
    }
};


