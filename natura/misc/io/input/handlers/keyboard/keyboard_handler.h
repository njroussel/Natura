#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../../../../observer_subject/subject.h"
#include "../../../../observer_subject/messages/keyboard_handler_message.h"

class KeyboardHandler : public Subject{
public:
    KeyboardHandler(GLFWwindow *window){
        glfwSetKeyCallback(window, keyCallback);
    }

    virtual void attach(Observer *obs){
        m_inner_subject.attach(obs);
    }

    virtual void notify(Message *msg){
        m_inner_subject.notify(msg);
    }

private:
    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods){
        Message *m = new KeyboardHandlerMessage(window, key, scancode, action, mods);
        m_inner_subject.notify(m);
        delete m;
    }

    static Subject m_inner_subject;
};

Subject KeyboardHandler::m_inner_subject;