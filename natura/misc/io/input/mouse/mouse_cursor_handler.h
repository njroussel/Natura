#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../../../observer_subject/subject.h"
#include "../handlers/handler.h"
#include "../../../observer_subject/messages/mouse_cursor_handler_message.h"


class MouseCursorHandler : public Handler{
public:
    MouseCursorHandler(GLFWwindow *window){
        glfwSetCursorPosCallback(window, mouseCursor);
    }

private:
    static void mouseCursor(GLFWwindow *window, double x, double y){
        Message *m = new MouseCursorHandlerMessage(window, x, y);
        m_inner_subject.notify(m);
        delete m;
    }
};