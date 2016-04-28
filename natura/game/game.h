#pragma once

#include "../projection.h"
#include "../perlin_noise/perlinnoise.h"
#include "../trackball.h"
#include "../../external/glm/detail/type_mat.hpp"
#include "../skybox/cube.h"
#include "../terrain/terrain.h"
#include <glm/gtc/matrix_transform.hpp>

class Game{
public:
    Game(GLFWwindow *window) {
        glfwGetWindowSize(window, &m_window_width, &m_window_height);
        m_window = window;
        m_amplitude = 1.05f;
        // set the callback for escape key
        glfwSetKeyCallback(m_window, keyCallback);

        // set the framebuffer resize callback
        glfwSetFramebufferSizeCallback(m_window, resize_callback);

        // set the mouse press and position callback
        glfwSetMouseButtonCallback(m_window, MouseButton);
        glfwSetCursorPosCallback(m_window, MousePos);

        Init();

        // update the window size with the framebuffer size (on hidpi screens the
        // framebuffer is bigger)
        glfwGetFramebufferSize(window, &m_window_width, &m_window_height);
        resize_callback(window, m_window_width, m_window_height);
    }
    ~Game() {
        m_perlinNoise->Cleanup();
        m_terrain->Cleanup();
        delete m_perlinNoise;
    }

    void run(){
        // render loop
        while (!glfwWindowShouldClose(m_window)) {
            Display();
            glfwSwapBuffers(m_window);
            glfwPollEvents();
        }
    }

private:
    /* Window size */
    static int m_window_width;
    static int m_window_height;
    static GLFWwindow *m_window;

    /* Camera and view */
    static glm::mat4 m_view_matrix;
    static glm::mat4 m_grid_model_matrix;
    static Projection *m_projection;

    static Trackball *m_trackball;

    /* Perlin noise generator for the game. */
    static PerlinNoise *m_perlinNoise;

    /* Terrain and sky */
    static Terrain *m_terrain;
    static float m_amplitude;
    static Cube *m_skybox;

    //TODO : Used for zoom - cleanup
    static float m_old_y;
    static glm::vec2 m_displ;


    /* Private function. */
    void Init() {
        m_trackball = new Trackball();
        m_projection = new Projection(45.0f, (GLfloat) m_window_width / m_window_height, 0.1f, 100.0f);
        m_perlinNoise = new PerlinNoise(m_window_width, m_window_height);
        m_terrain = new Terrain(2, 64, m_perlinNoise);
        m_skybox = new Cube();


        // sets background color b
        glClearColor(0, 0, 0/*gray*/, 1.0 /*solid*/);

        // enable depth test.
        glEnable(GL_DEPTH_TEST);

        m_view_matrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, -4.0f));

        m_grid_model_matrix = translate(m_grid_model_matrix, vec3(-4.0f, -0.25f, -4.0f));
        m_grid_model_matrix = scale(m_grid_model_matrix, vec3(2.0, 2.0, 2.0f));

        //int perlinNoiseTex = perlinNoise.generateNoise(H, frequency, lacunarity, offset, octaves);
        m_terrain->Init(/*perlinNoiseTex*/);
        m_skybox->Init();
    }

    void Display() {
        glViewport(0, 0, m_window_width, m_window_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const float time = glfwGetTime();

        m_terrain->Draw(m_amplitude, time, m_trackball->matrix() * m_grid_model_matrix, m_view_matrix, m_projection->perspective());
        m_skybox->Draw(m_projection->perspective() * m_view_matrix * m_trackball->matrix());
    }

    // transforms glfw screen coordinates into normalized OpenGL coordinates.
    static vec2 TransformScreenCoords(GLFWwindow *window, int x, int y) {
        // the framebuffer and the window doesn't necessarily have the same size
        // i.e. hidpi screens. so we need to get the correct one
        int width;
        int height;
        glfwGetWindowSize(window, &width, &height);
        return vec2(2.0f * (float) x / width - 1.0f,
                    1.0f - 2.0f * (float) y / height);
    }

    static void MouseButton(GLFWwindow *window, int button, int action, int mod) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            double x_i, y_i;
            glfwGetCursorPos(window, &x_i, &y_i);
            vec2 p = TransformScreenCoords(window, x_i, y_i);
            m_trackball->BeginDrag(p.x, p.y);
        }

        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
            double x_i, y_i;
            glfwGetCursorPos(window, &x_i, &y_i);
            vec2 p = TransformScreenCoords(window, x_i, y_i);
            // Store the current state of the y position matrix.
            m_old_y = p.y;
        }
    }

    static void MousePos(GLFWwindow *window, double x, double y) {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            vec2 p = TransformScreenCoords(window, x, y);
            m_trackball->recomputeMatrixAfterDrag(p.x, p.y);
        }

        // zoom
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            vec2 p = TransformScreenCoords(window, x, y);
            float newZoom = p.y - m_old_y;
            m_view_matrix = glm::translate(m_view_matrix, vec3(0, 0, newZoom * 5.0));
            m_old_y = p.y;
        }
    }

    // Gets called when the windows/framebuffer is resized.
    static void resize_callback(GLFWwindow *window, int width, int height) {
        m_window_width = width;
        m_window_height = height;
        m_projection->reGenerateMatrix((GLfloat) m_window_width / m_window_height);
        glViewport(0, 0, m_window_width, m_window_height);
        m_terrain->Refresh(m_perlinNoise->generateNoise(m_displ));
    }

    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
            m_displ.x -= 0.2;
        }
        if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
            m_displ.x += 0.2;
        }
        if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
            m_displ.y -= 0.2;
        }
        if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
            m_displ.y += 0.2;
        }
        cout << "Displ  : " << m_displ.x << " ; " << m_displ.y << endl;
        m_terrain->Refresh(m_perlinNoise->generateNoise(m_displ));
    }
};

int Game::m_window_width;
int Game::m_window_height;
GLFWwindow *Game::m_window;

/* Camera and view */
glm::mat4 Game::m_view_matrix;
glm::mat4 Game::m_grid_model_matrix;
Projection *Game::m_projection;

Trackball *Game::m_trackball;

/* Perlin noise generator for the game. */
PerlinNoise *Game::m_perlinNoise;

/* Terrain and sky */
Terrain *Game::m_terrain;
float Game::m_amplitude;
Cube *Game::m_skybox;

//TODO : Used for zoom - cleanup
float Game::m_old_y;
glm::vec2 Game::m_displ;