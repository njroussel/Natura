#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../../../observer_subject/subject.h"
#include "../../../observer_subject/messages/keyboard_handler_message.h"
#include "../handlers/handler.h"
#include "../../../observer_subject/messages/mouse_button_handler_message.h"


class MouseButtonHandler : public Handler{
public:
    MouseButtonHandler(GLFWwindow *window){
        glfwSetMouseButtonCallback(window, mouseButton);
    }

private:
    static void mouseButton(GLFWwindow *window, int button, int action, int mod){
        Message *m = new MouseButtonHandlerMessage(window, button, action, mod);
        m_inner_subject.notify(m);
        delete m;
    }
};