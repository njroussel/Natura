#ifndef NATURA_GAME_H
#define NATURA_GAME_H

#include "../projection.h"
#include "../perlin_noise/perlinnoise.h"
#include "../trackball.h"
#include "../../external/glm/detail/type_mat.hpp"
#include <glm/gtc/matrix_transform.hpp>

class Game{
public:
    Game(GLFWwindow *window) {
        glfwGetWindowSize(window, &m_window_width, &m_window_height);
        m_window = window;
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
    int m_window_width;
    int m_window_height;
    GLFWwindow *m_window;

    /* Camera and view */
    glm::mat4 m_view_matrix;
    glm::mat4 m_grid_model_matrix;
    Projection *m_projection;

    Trackball *m_trackball;

    /* Perlin noise generator for the game. */
    PerlinNoise *m_perlinNoise;

    /* Terrain and sky */
    Terrain *m_terrain;
    float m_amplitude = 1.05f;
    Cube *m_skybox;

    //TODO : Used for zoom - cleanup
    float m_old_y;
    glm::vec2 m_displ;


    /* Private function. */
    void Init() {
        m_trackball = new Trackball();
        m_projection = new Projection(45.0f, (GLfloat) m_window_width / m_window_height, 0.1f, 100.0f);
        m_perlinNoise = new PerlinNoise(m_window_width, m_window_height);
        m_terrain = new Terrain(4, 64, m_perlinNoise);


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
    vec2 TransformScreenCoords(GLFWwindow *window, int x, int y) {
        // the framebuffer and the window doesn't necessarily have the same size
        // i.e. hidpi screens. so we need to get the correct one
        int width;
        int height;
        glfwGetWindowSize(window, &width, &height);
        return vec2(2.0f * (float) x / width - 1.0f,
                    1.0f - 2.0f * (float) y / height);
    }

    void MouseButton(GLFWwindow *window, int button, int action, int mod) {
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

    void MousePos(GLFWwindow *window, double x, double y) {
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
    void resize_callback(GLFWwindow *window, int width, int height) {
        m_window_width = width;
        m_window_height = height;
        m_projection->reGenerateMatrix((GLfloat) m_window_width / m_window_height);
        glViewport(0, 0, m_window_width, m_window_height);
        m_terrain->Refresh(m_perlinNoise->generateNoise(m_displ));
    }

    void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        /*if (key == GLFW_KEY_H && action == GLFW_PRESS) {
            H += 0.05f;
        }
        if (key == GLFW_KEY_N && action == GLFW_PRESS) {
            H -= 0.05f;
        }
        if (key == GLFW_KEY_F && action == GLFW_PRESS) {
            frequency += 0.1f;
        }
        if (key == GLFW_KEY_V && action == GLFW_PRESS) {
            frequency -= 0.1f;
        }
        if (key == GLFW_KEY_O && action == GLFW_PRESS) {
            offset += 0.05;
        }
        if (key == GLFW_KEY_L && action == GLFW_RELEASE) {
            offset -= 0.05;
        }
        if (key == GLFW_KEY_I && action == GLFW_PRESS) {
            lacunarity += 0.05f;
        }
        if (key == GLFW_KEY_K && action == GLFW_PRESS) {
            lacunarity -= 0.05f;
        }
        if (key == GLFW_KEY_A && action == GLFW_PRESS) {
            amplitude += 0.1f;
        }
        if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
            amplitude -= 0.1f;
        }
        if ((key >= 49 && key <= 57) && action == GLFW_PRESS) {
            octaves = key - 49;
        }*/
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
        /*if (key == GLFW_KEY_J && action == GLFW_PRESS) {
            octaves --;
        }
        if (key == GLFW_KEY_U && action == GLFW_PRESS) {
            octaves ++;
        }*/
        cout << "Displ  : " << m_displ.x << " ; " << m_displ.y << endl;
        m_terrain->Refresh(m_perlinNoise->generateNoise(m_displ));
    }
};

#endif //NATURA_GAME_H
