#pragma once

class Mouse {

    static void MouseButton(GLFWwindow *window, int button, int action, int mod) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            double x_i, y_i;
            glfwGetCursorPos(window, &x_i, &y_i);
            vec2 p = TransformScreenCoords(window, x_i, y_i);
            trackball.BeginDrag(p.x, p.y);
        }

        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
            double x_i, y_i;
            glfwGetCursorPos(window, &x_i, &y_i);
            vec2 p = TransformScreenCoords(window, x_i, y_i);
            // Store the current state of the y position matrix.
            old_y = p.y;
        }
    }

    static void MousePos(GLFWwindow *window, double x, double y) {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            vec2 p = TransformScreenCoords(window, x, y);
            trackball.recomputeMatrixAfterDrag(p.x, p.y);
        }

        // zoom
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            vec2 p = TransformScreenCoords(window, x, y);
            float newZoom = p.y - old_y;
            view_matrix = glm::translate(view_matrix, vec3(0, 0, newZoom * 5.0));
            old_y = p.y;
        }
    }

private :
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
};


