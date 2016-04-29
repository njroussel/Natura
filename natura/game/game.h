#pragma once

#include "../projection.h"
#include "../perlin_noise/perlinnoise.h"
#include "../trackball.h"
#include "../../external/glm/detail/type_mat.hpp"
#include "../skybox/cube.h"
#include "../terrain/terrain.h"
#include "../misc/observer_subject/messages/keyboard_handler_message.h"
#include "../misc/io/input/keyboard/keyboard_handler.h"
#include "../misc/io/input/mouse/mouse_button_handler.h"
#include <glm/gtc/matrix_transform.hpp>

class Game : public Observer{
public:
    Game(GLFWwindow *window) : m_keyboard_handler(window), m_mouse_button_handler(window){
        glfwGetWindowSize(window, &m_window_width, &m_window_height);
        m_window = window;
        m_amplitude = 1.05f;
        // set the callback for escape key
//        glfwSetKeyCallback(m_window, keyCallback);

        // set the framebuffer resize callback
        glfwSetFramebufferSizeCallback(m_window, resize_callback);

        // set the mouse press and position callback
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
        m_keyboard_handler.attach(this);
        m_mouse_button_handler.attach(this);
        // render loop
        while (!glfwWindowShouldClose(m_window)) {
            Display();
            glfwSwapBuffers(m_window);
            glfwPollEvents();
        }
    }

    void update(Message *msg){
        switch (msg->getType()) {
            case Message::Type::KEYBOARD_HANDLER_INPUT :
                keyCallback(reinterpret_cast<KeyboardHandlerMessage *>(msg));
                break;

            case Message::Type::MOUSE_BUTTON_INPUT :
                mouseButtonCallback(
                        reinterpret_cast<MouseButtonHandlerMessage *>(msg));
                break;
            default:
                throw std::string("Error : Unexpected message in class Game");
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

    //TODO : Used for zoom - cleanup
    static float m_old_y;
    static glm::vec2 m_displ;

    /* Input handlers */
    KeyboardHandler m_keyboard_handler;
    MouseButtonHandler m_mouse_button_handler;


    /* Private function. */
    void Init() {
        m_trackball = new Trackball();
        m_projection = new Projection(45.0f, (GLfloat) m_window_width / m_window_height, 0.1f, 100.0f);
        m_perlinNoise = new PerlinNoise(m_window_width, m_window_height);
        m_terrain = new Terrain(1, 64, m_perlinNoise);


        // sets background color b
        glClearColor(0, 0, 0/*gray*/, 1.0 /*solid*/);

        // enable depth test.
        glEnable(GL_DEPTH_TEST);

        m_view_matrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, -4.0f));

        m_grid_model_matrix = translate(m_grid_model_matrix, vec3(-4.0f, -0.25f, -4.0f));
        m_grid_model_matrix = scale(m_grid_model_matrix, vec3(2.0, 2.0, 2.0f));

        //int perlinNoiseTex = perlinNoise.generateNoise(H, frequency, lacunarity, offset, octaves);
        m_terrain->Init(/*perlinNoiseTex*/);
    }

    void Display() {
        glViewport(0, 0, m_window_width, m_window_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const float time = glfwGetTime();

        m_terrain->Draw(m_amplitude, time, m_trackball->matrix() * m_grid_model_matrix, m_view_matrix, m_projection->perspective());
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

    void mouseButtonCallback(MouseButtonHandlerMessage *message) {
        int button = message->getButton();
        int action = message->getAction();
        GLFWwindow *window = message->getWindow();
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
    }

    void keyCallback(KeyboardHandlerMessage *message) {
        GLFWwindow *window = message->getWindow();
        int key = message->getKey();
        int action = message->getAction();
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            switch (key) {
                case GLFW_KEY_LEFT:
                    m_displ.x -= 0.2;
                    break;

                case GLFW_KEY_RIGHT:
                    m_displ.x += 0.2;
                    break;

                case GLFW_KEY_UP:
                    m_displ.y -= 0.2;
                    break;

                case GLFW_KEY_DOWN:
                    m_displ.y += 0.2;
                    break;

                case GLFW_KEY_ESCAPE:
                    glfwSetWindowShouldClose(window, GL_TRUE);
                    break;

                case GLFW_KEY_H:
                    m_perlinNoise->setProperty(PerlinNoiseProperty::H, m_perlinNoise->getProperty(PerlinNoiseProperty::H) + .05f);
                    break;

                case GLFW_KEY_N:
                    m_perlinNoise->setProperty(PerlinNoiseProperty::H, m_perlinNoise->getProperty(PerlinNoiseProperty::H) - .05f);
                    break;

                case GLFW_KEY_F:
                    m_perlinNoise->setProperty(PerlinNoiseProperty::FREQUENCY, m_perlinNoise->getProperty(PerlinNoiseProperty::FREQUENCY) + 0.1f);
                    break;

                case GLFW_KEY_V:
                    m_perlinNoise->setProperty(PerlinNoiseProperty::FREQUENCY, m_perlinNoise->getProperty(PerlinNoiseProperty::FREQUENCY) - 0.1f);
                    break;

                case GLFW_KEY_O:
                    m_perlinNoise->setProperty(PerlinNoiseProperty::OFFSET, m_perlinNoise->getProperty(PerlinNoiseProperty::OFFSET) + 0.05);
                    break;

                case GLFW_KEY_L:
                    m_perlinNoise->setProperty(PerlinNoiseProperty::OFFSET, m_perlinNoise->getProperty(PerlinNoiseProperty::OFFSET) - 0.05);
                    break;

                case GLFW_KEY_I:
                    m_perlinNoise->setProperty(PerlinNoiseProperty::LACUNARITY, m_perlinNoise->getProperty(PerlinNoiseProperty::LACUNARITY) + 0.05f);
                    break;

                case GLFW_KEY_K:
                    m_perlinNoise->setProperty(PerlinNoiseProperty::LACUNARITY, m_perlinNoise->getProperty(PerlinNoiseProperty::LACUNARITY) - 0.05f);
                    break;

                case GLFW_KEY_A:
                    m_amplitude += 0.1f;
                    break;

                case GLFW_KEY_Z:
                    m_amplitude -= 0.1f;
                    break;

                case GLFW_KEY_J:
                    m_perlinNoise->setProperty(PerlinNoiseProperty::OCTAVE, m_perlinNoise->getProperty(PerlinNoiseProperty::OCTAVE) - 1);
                    break;

                case GLFW_KEY_U:
                    m_perlinNoise->setProperty(PerlinNoiseProperty::OCTAVE, m_perlinNoise->getProperty(PerlinNoiseProperty::OCTAVE) + 1);
                    break;

                default:break;
            }
        }
        cout << "Displ  : " << m_displ.x << " ; " << m_displ.y << endl;
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

//TODO : Used for zoom - cleanup
float Game::m_old_y;
glm::vec2 Game::m_displ;
