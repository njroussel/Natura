#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../../../../observer_subject/subject.h"
#include "../../../../observer_subject/messages/keyboard_handler_message.h"
#include "../handler.h"

class KeyboardHandler : public Handler{
public:
    KeyboardHandler(GLFWwindow *window){
        glfwSetKeyCallback(window, keyCallback);
    }

private:
    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods){
        Message *m = new KeyboardHandlerMessage(window, key, scancode, action, mods);
        m_inner_subject.notify(m);
        delete m;
    }
};