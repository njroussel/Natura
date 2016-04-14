// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>
#include "terrain/terrain.h"
#include "trackball.h"
#include "perlin_noise/perlinnoise.h"
#include "projection.h"
#include "keyboard.h"

using namespace glm;

Terrain terrain(512);

int window_width = 800;
int window_height = 600;


mat4 view_matrix;
mat4 grid_model_matrix;

Trackball *trackball;
PerlinNoise perlinNoise(window_width, window_height);
Projection *projection;

//TODO : Used for zoom - cleanup
float old_y;


//calibration values
float H = 0.4f;
float lacunarity = 2.0f;
float offset = 1.0f;
float frequency = 0.64f;
int octaves = 6;
float amplitude = 0.95f;

void Init() {
    trackball = new Trackball();
    projection = new Projection(45.0f, (GLfloat) window_width / window_height, 0.1f, 100.0f);

    // sets background color b
    glClearColor(0, 0, 0/*gray*/, 1.0 /*solid*/);

    // enable depth test.
    glEnable(GL_DEPTH_TEST);

    view_matrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, -4.0f));

    grid_model_matrix = translate(mat4(1.0f), vec3(0.0f, -0.25f, 0.0f));
    grid_model_matrix = translate(grid_model_matrix, vec3(-1.0f, 0.0f, -1.0f));
    grid_model_matrix = scale(grid_model_matrix, vec3(2.0, 1.0, 2.0f));

    int perlinNoiseTex = perlinNoise.generateNoise(H, frequency, lacunarity, offset, octaves);
    terrain.Init(perlinNoiseTex);
}

void Display() {
    glViewport(0, 0, window_width, window_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const float time = glfwGetTime();

    terrain.Draw(amplitude, time, trackball->matrix() * grid_model_matrix, view_matrix, projection->perspective());
    //perlinNoise.Draw(H);
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
        trackball->BeginDrag(p.x, p.y);
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        double x_i, y_i;
        glfwGetCursorPos(window, &x_i, &y_i);
        vec2 p = TransformScreenCoords(window, x_i, y_i);
        // Store the current state of the y position matrix.
        old_y = p.y;
    }
}

void MousePos(GLFWwindow *window, double x, double y) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        vec2 p = TransformScreenCoords(window, x, y);
        trackball->recomputeMatrixAfterDrag(p.x, p.y);
    }

    // zoom
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        vec2 p = TransformScreenCoords(window, x, y);
        float newZoom = p.y - old_y;
        view_matrix = glm::translate(view_matrix, vec3(0, 0, newZoom * 5.0));
        old_y = p.y;
    }
}

// Gets called when the windows/framebuffer is resized.
void resize_callback(GLFWwindow *window, int width, int height) {
    window_width = width;
    window_height = height;
    projection->reGenerateMatrix((GLfloat) window_width / window_height);
    glViewport(0, 0, window_width, window_height);
    perlinNoise.refreshNoise(window_width, window_height, H, frequency, lacunarity, offset, octaves);
}

void ErrorCallback(int error, const char *description) {
    fputs(description, stderr);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key == GLFW_KEY_H && action == GLFW_PRESS) {
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
    }
    cout << "H : " << H << endl;
    cout << "Lacunarity : " << lacunarity << endl;
    cout << "Offset : " << offset << endl;
    cout << "Frequency : " << frequency << endl;
    cout << "Octaves : " << octaves << endl;
    cout << "Amplitude : " << amplitude << endl;
    perlinNoise.refreshNoise(window_width, window_height, H, frequency, lacunarity, offset, octaves);
    //Just acces mKeyMap and call the callback function.
}

int main(int argc, char *argv[]) {
    // GLFW Initialization
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return EXIT_FAILURE;
    }

    glfwSetErrorCallback(ErrorCallback);

    // hint GLFW that we would like an OpenGL 3 context (at least)
    // http://www.glfw.org/faq.html#how-do-i-create-an-opengl-30-context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // attempt to open the window: fails if required version unavailable
    // note some Intel GPUs do not support OpenGL 3.2
    // note update the driver of your graphic card
    GLFWwindow *window = glfwCreateWindow(window_width, window_height,
                                          "Trackball", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // makes the OpenGL context of window current on the calling thread
    glfwMakeContextCurrent(window);

    // set the callback for escape key
    glfwSetKeyCallback(window, keyCallback);

    // set the framebuffer resize callback
    glfwSetFramebufferSizeCallback(window, resize_callback);

    // set the mouse press and position callback
    glfwSetMouseButtonCallback(window, MouseButton);
    glfwSetCursorPosCallback(window, MousePos);

    // GLEW Initialization (must have a context)
    // https://www.opengl.org/wiki/OpenGL_Loading_Library
    glewExperimental = GL_TRUE; // fixes glew error (see above link)
    if (glewInit() != GLEW_NO_ERROR) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return EXIT_FAILURE;
    }

    cout << "OpenGL" << glGetString(GL_VERSION) << endl;

    // initialize our OpenGL program
    Init();

    // update the window size with the framebuffer size (on hidpi screens the
    // framebuffer is bigger)
    glfwGetFramebufferSize(window, &window_width, &window_height);
    resize_callback(window, window_width, window_height);

    // render loop
    while (!glfwWindowShouldClose(window)) {
        Display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    perlinNoise.Cleanup();
    terrain.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
