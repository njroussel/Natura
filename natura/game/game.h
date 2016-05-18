#pragma once

#include "../projection.h"
#include "../camera/camera.h"
#include "../perlin_noise/perlinnoise.h"
#include "../trackball.h"
#include "../../external/glm/detail/type_mat.hpp"
#include "../skybox/cube.h"
#include "../terrain/terrain.h"
#include "../misc/observer_subject/messages/keyboard_handler_message.h"
#include "../misc/io/input/handlers/keyboard/keyboard_handler.h"
#include "../misc/io/input/handlers/mouse/mouse_button_handler.h"
#include "../misc/io/input/handlers/mouse/mouse_cursor_handler.h"
#include "../misc/io/input/handlers/framebuffer/framebuffer_size_handler.h"
#include <glm/gtc/matrix_transform.hpp>


class Game : public Observer {
public:
    Game(GLFWwindow *window) : m_keyboard_handler(window), m_mouse_button_handler(window),
                               m_mouse_cursor_handler(window), m_frame_buffer_size_handler(window) {
        glfwGetWindowSize(window, &m_window_width, &m_window_height);
        m_window = window;
        m_amplitude = 9.05f;
        Init();
        glfwGetFramebufferSize(window, &m_window_width, &m_window_height);
        FrameBufferSizeHandlerMessage m(window, m_window_width, m_window_height);
        resize_callback(&m);
    }

    ~Game() {
        m_perlinNoise->Cleanup();
        m_terrain->Cleanup();
        delete m_perlinNoise;
    }

    void run() {
        m_keyboard_handler.attach(this);
        m_mouse_button_handler.attach(this);
        m_mouse_cursor_handler.attach(this);
        m_frame_buffer_size_handler.attach(this);
        // render loop
        while (!glfwWindowShouldClose(m_window)) {
            Display();
            glfwSwapBuffers(m_window);
            glfwPollEvents();
        }
    }

    void update(Message *msg) {
        switch (msg->getType()) {
            case Message::Type::KEYBOARD_HANDLER_INPUT :
                keyCallback(reinterpret_cast<KeyboardHandlerMessage *>(msg));
                break;

            case Message::Type::MOUSE_BUTTON_INPUT :
                mouseButtonCallback(
                        reinterpret_cast<MouseButtonHandlerMessage *>(msg));
                break;

            case Message::Type::MOUSE_CURSOR_INPUT :
                mouseCursorCallback(reinterpret_cast<MouseCursorHandlerMessage *>(msg));
                break;

            case Message::Type::FRAMEBUFFER_SIZE_CHANGE :
                resize_callback(reinterpret_cast<FrameBufferSizeHandlerMessage *>(msg));
                break;

            default:
                throw std::string("Error : Unexpected message in class Game");
        }
    }

private:
    double m_last_mouse_xpos, m_last_mouse_ypos;
    float m_last_time;

    /* Window size */
    int m_window_width;
    int m_window_height;
    GLFWwindow *m_window;

    /* Camera and view */
    Camera *m_camera;
    glm::mat4 m_grid_model_matrix;
    Projection *m_projection;

    Trackball *m_trackball;

    /* Perlin noise generator for the game. */
    PerlinNoise *m_perlinNoise;

    /* Terrain and sky */
    Terrain *m_terrain;
    float m_amplitude;

    glm::vec2 m_displ;

    /* Input handlers */
    KeyboardHandler m_keyboard_handler;
    MouseButtonHandler m_mouse_button_handler;
    MouseCursorHandler m_mouse_cursor_handler;
    FrameBufferSizeHandler m_frame_buffer_size_handler;



    /* Private function. */
    void Init() {
        const bool top_down_view = true;
        const int TERRAIN_SIZE = 10;
        const int VERT_PER_GRID_SIDE = 8;
        const float cam_posxy = TERRAIN_SCALE * ((float)(TERRAIN_SIZE * CHUNK_SIDE_TILE_COUNT)) / 2.0f;
        cout << "Campos : " << cam_posxy << endl;
        vec3 starting_camera_position;
        vec2 starting_camera_rotation;
        if (top_down_view) {
            starting_camera_position = vec3(0, -0.5, -0.5);
            starting_camera_rotation = vec2(0.0f, 40.f);
        }
        else {
            starting_camera_position = vec3(-cam_posxy, -5.0f, -cam_posxy);
            starting_camera_rotation = vec2(0.0f);
        }
        m_camera = new Camera(starting_camera_position, starting_camera_rotation);
        m_trackball = new Trackball();
        m_projection = new Projection(45.0f, (GLfloat) m_window_width / m_window_height, 0.1f, 100.0f);
        m_perlinNoise = new PerlinNoise(m_window_width, m_window_height, glm::vec2(TERRAIN_SIZE, TERRAIN_SIZE));
        m_terrain = new Terrain(TERRAIN_SIZE, VERT_PER_GRID_SIDE, m_perlinNoise);


        // sets background color b
        glClearColor(0, 0, 0/*gray*/, 1.0 /*solid*/);

        // enable depth test.
        glEnable(GL_DEPTH_TEST);
        m_grid_model_matrix = IDENTITY_MATRIX;
        //m_grid_model_matrix = translate(m_grid_model_matrix, vec3(-4.0f, -0.25f, -4.0f));
        m_grid_model_matrix = scale(m_grid_model_matrix, vec3(TERRAIN_SCALE, TERRAIN_SCALE, TERRAIN_SCALE));

        //int perlinNoiseTex = perlinNoise.generateNoise(H, frequency, lacunarity, offset, octaves);
        BASE_TILE = new Grid(VERT_PER_GRID_SIDE, glm::vec2(0, 0));
        BASE_TILE->Init(0);
        m_perlinNoise->Init();
        m_terrain->Init(/*perlinNoiseTex*/);
    }

    void Display() {
        glViewport(0, 0, m_window_width, m_window_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const float time = glfwGetTime();


        //tick 60 times per second
        if (time - m_last_time > 1 / 60.0f) {
            m_camera->CalculateMatrix();
            m_last_time = time;
        }

        //draw as often as possible
        m_terrain->ExpandTerrain(m_camera->getPosition());
        //m_terrain->_expand(Terrain::Direction::SOUTH);
        m_terrain->Draw(m_amplitude, time, m_camera->getPosition(), m_grid_model_matrix, m_camera->GetMatrix(),
                        m_projection->perspective());

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
            m_last_mouse_xpos = x_i;
            m_last_mouse_ypos = y_i;
        }
    }

    void mouseCursorCallback(MouseCursorHandlerMessage *message) {
        GLFWwindow *window = message->getWindow();
        double x = message->getCoordX();
        double y = message->getCoordY();
        if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            double diffx = x - m_last_mouse_xpos; //check the difference between the current x and the last x position
            double diffy = y - m_last_mouse_ypos; //check the difference between the current y and the last y position
            m_last_mouse_xpos = x; //set lastx to the current x position
            m_last_mouse_ypos = y; //set lasty to the current y position
            float xrot =
                    (float) diffy * 0.1f; //set the xrot to xrot with the addition of the difference in the y position
            float yrot =
                    (float) diffx * 0.1f;// set the xrot to yrot with the addition of the difference in the x position
            vec2 tmp = vec2(xrot, yrot);
            m_camera->AddRotation(tmp);
        }
    }

    // Gets called when the windows/framebuffer is resized.
    void resize_callback(FrameBufferSizeHandlerMessage *message) {
        int width = message->getWidth();
        int height = message->getHeight();
        m_window_width = width;
        m_window_height = height;
        m_projection->reGenerateMatrix((GLfloat) m_window_width / m_window_height);
        glViewport(0, 0, m_window_width, m_window_height);
    }

    void keyCallback(KeyboardHandlerMessage *message) {
        GLFWwindow *window = message->getWindow();
        int key = message->getKey();
        int action = message->getAction();
        if (action == GLFW_PRESS) {
            switch (key) {
                case GLFW_KEY_W: {
                    m_camera->SetMovement(DIRECTION::Forward, true);
                    break;
                }
                case GLFW_KEY_S : {
                    m_camera->SetMovement(DIRECTION::Backward, true);
                    break;

                }
            }
        }
        if (action == GLFW_RELEASE) {
            switch (key) {
                case GLFW_KEY_W: {
                    m_camera->SetMovement(DIRECTION::Forward, false);
                    break;
                }
                case GLFW_KEY_S : {
                    m_camera->SetMovement(DIRECTION::Backward, false);
                    break;

                }
                /*case GLFW_KEY_LEFT:
                    m_terrain->_expand(Terrain::Direction::WEST);
                    break;

                case GLFW_KEY_RIGHT:
                    m_terrain->_expand(Terrain::Direction::EST);
                    break;

                case GLFW_KEY_UP:
                    m_terrain->_expand(Terrain::Direction::NORTH);
                    break;

                case GLFW_KEY_DOWN:
                    m_terrain->_expand(Terrain::Direction::SOUTH);
                    break;*/
            }
        }
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            switch (key) {
                case GLFW_KEY_ESCAPE:
                    glfwSetWindowShouldClose(window, GL_TRUE);
                    break;

                case GLFW_KEY_H:
                    m_perlinNoise->setProperty(PerlinNoiseProperty::H,
                                               m_perlinNoise->getProperty(PerlinNoiseProperty::H) + .05f);
                    break;

                case GLFW_KEY_N:
                    m_perlinNoise->setProperty(PerlinNoiseProperty::H,
                                               m_perlinNoise->getProperty(PerlinNoiseProperty::H) - .05f);
                    break;

                case GLFW_KEY_F:
                    m_perlinNoise->setProperty(PerlinNoiseProperty::FREQUENCY,
                                               m_perlinNoise->getProperty(PerlinNoiseProperty::FREQUENCY) + 0.1f);
                    break;

                case GLFW_KEY_V:
                    m_perlinNoise->setProperty(PerlinNoiseProperty::FREQUENCY,
                                               m_perlinNoise->getProperty(PerlinNoiseProperty::FREQUENCY) - 0.1f);
                    break;

                case GLFW_KEY_O:
                    m_perlinNoise->setProperty(PerlinNoiseProperty::OFFSET,
                                               m_perlinNoise->getProperty(PerlinNoiseProperty::OFFSET) + 0.05);
                    break;

                case GLFW_KEY_L:
                    m_perlinNoise->setProperty(PerlinNoiseProperty::OFFSET,
                                               m_perlinNoise->getProperty(PerlinNoiseProperty::OFFSET) - 0.05);
                    break;

                case GLFW_KEY_I:
                    m_perlinNoise->setProperty(PerlinNoiseProperty::LACUNARITY,
                                               m_perlinNoise->getProperty(PerlinNoiseProperty::LACUNARITY) + 0.05f);
                    break;

                case GLFW_KEY_K:
                    m_perlinNoise->setProperty(PerlinNoiseProperty::LACUNARITY,
                                               m_perlinNoise->getProperty(PerlinNoiseProperty::LACUNARITY) - 0.05f);
                    break;

                case GLFW_KEY_Z:
                    m_amplitude += 0.1f;
                    break;

                case GLFW_KEY_X:
                    m_amplitude -= 0.1f;
                    break;

                case GLFW_KEY_J:
                    m_perlinNoise->setProperty(PerlinNoiseProperty::OCTAVE,
                                               m_perlinNoise->getProperty(PerlinNoiseProperty::OCTAVE) - 1);
                    break;

                case GLFW_KEY_U:
                    m_perlinNoise->setProperty(PerlinNoiseProperty::OCTAVE,
                                               m_perlinNoise->getProperty(PerlinNoiseProperty::OCTAVE) + 1);
                    break;

                case GLFW_KEY_G:
                    m_terrain->water_height += 0.05f;
                    break;

                case GLFW_KEY_B:
                    m_terrain->water_height -= 0.05f;
                    break;

                case GLFW_KEY_SPACE:
                    cout << "KEKED" << endl;
                    m_camera->LookAt(glm::vec3(0, 0, 0));
                    break;

                case GLFW_KEY_A : {
                    glm::vec2 rot = vec2(0, -1);
                    m_camera->AddRotation(rot);
                    break;
                }
                case GLFW_KEY_D: {
                    glm::vec2 rot = vec2(0, 1);
                    m_camera->AddRotation(rot);
                    break;
                }

                case GLFW_KEY_Q : {
                    glm::vec2 rot = vec2(-1, 0);
                    m_camera->AddRotation(rot);
                    break;
                }
                case GLFW_KEY_E: {
                    glm::vec2 rot = vec2(1, 0);
                    m_camera->AddRotation(rot);
                    break;
                }

                default:
                    break;
            }
            /*cout << "Ampl.  : " << m_amplitude << endl;
            cout << "H      : " << m_perlinNoise->getProperty(PerlinNoiseProperty::H) << endl;
            cout << "Freq   : " << m_perlinNoise->getProperty(PerlinNoiseProperty::FREQUENCY) << endl;
            cout << "Offset : " << m_perlinNoise->getProperty(PerlinNoiseProperty::OFFSET) << endl;
            cout << "Lac.   : " << m_perlinNoise->getProperty(PerlinNoiseProperty::LACUNARITY) << endl;
            cout << "Octave : " << m_perlinNoise->getProperty(PerlinNoiseProperty::OCTAVE) << endl;
            cout << "Water h: " << m_terrain->water_height << endl;*/
        }
    }
};