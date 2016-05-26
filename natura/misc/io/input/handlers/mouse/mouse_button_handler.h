#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../../../../observer_subject/subject.h"
#include "../../../../observer_subject/messages/keyboard_handler_message.h"
#include "../../../../observer_subject/messages/mouse_button_handler_message.h"


class MouseButtonHandler : public Subject{
public:
    MouseButtonHandler(GLFWwindow *window){
        glfwSetMouseButtonCallback(window, mouseButton);
    }

    virtual void attach(Observer *obs){
        m_inner_subject.attach(obs);
    }

    virtual void notify(Message *msg){
        m_inner_subject.notify(msg);
    }

private:
    static void mouseButton(GLFWwindow *window, int button, int action, int mod){
        Message *m = new MouseButtonHandlerMessage(window, button, action, mod);
        m_inner_subject.notify(m);
        delete m;
    }

    static Subject m_inner_subject;
};

Subject MouseButtonHandler::m_inner_subject;