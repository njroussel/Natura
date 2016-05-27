#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "icg_helper.h"
#include "../physics/material_point.h"

class Ball : public MaterialPoint {

    Ball(vec3 &starting_position, vec3 starting_vector) : MaterialPoint(1.0, starting_position) {
        m_position = starting_position;
        string error;
        // obj files can contains material informations
        vector<tinyobj::material_t> materials;
        string filename = "shpere.obj";

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

    }

    void tick() {

    }

private:

    vec3 m_position;
    std::vector<tinyobj::shape_t> m_shapes;
    GLuint m_vertex_buffer_object;           // memory buffer
    GLuint m_vertex_normal_buffer_object;    // memory buffer
    GLuint m_vertex_array_id;                // vertex array object
    GLuint m_program_id;
};

