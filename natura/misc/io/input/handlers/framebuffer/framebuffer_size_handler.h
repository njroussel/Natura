#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../../../../observer_subject/subject.h"
#include "../handler.h"
#include "../../../../observer_subject/messages/framebuffer_size_handler_message.h"


class FrameBufferSizeHandler : public Handler{
public:
    FrameBufferSizeHandler(GLFWwindow *window){
        glfwSetFramebufferSizeCallback(window, framebufferSizeChange);
    }

private:
    static void framebufferSizeChange(GLFWwindow *window, int width, int height){
        Message *m = new FrameBufferSizeHandlerMessage(window, width, height);
        m_inner_subject.notify(m);
        delete m;
    }
};