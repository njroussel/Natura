#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../../../../observer_subject/subject.h"
#include "../../../../observer_subject/messages/mouse_cursor_handler_message.h"


class MouseCursorHandler : public Subject{
public:
    MouseCursorHandler(GLFWwindow *window){
        glfwSetCursorPosCallback(window, mouseCursor);
    }

    virtual void attach(Observer *obs){
        m_inner_subject.attach(obs);
    }

    virtual void notify(Message *msg){
        m_inner_subject.notify(msg);
    }

private:
    static void mouseCursor(GLFWwindow *window, double x, double y){
        Message *m = new MouseCursorHandlerMessage(window, x, y);
        m_inner_subject.notify(m);
        delete m;
    }

    static Subject m_inner_subject;
};

Subject MouseCursorHandler::m_inner_subject;