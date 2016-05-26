#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../../../../observer_subject/subject.h"
#include "../../../../observer_subject/messages/framebuffer_size_handler_message.h"


class FrameBufferSizeHandler : public Subject{
public:
    FrameBufferSizeHandler(GLFWwindow *window){
        glfwSetFramebufferSizeCallback(window, framebufferSizeChange);
    }

    virtual void attach(Observer *obs){
        m_inner_subject.attach(obs);
    }

    virtual void notify(Message *msg){
        m_inner_subject.notify(msg);
    }

private:
    static void framebufferSizeChange(GLFWwindow *window, int width, int height){
        Message *m = new FrameBufferSizeHandlerMessage(window, width, height);
        m_inner_subject.notify(m);
        delete m;
    }

    static Subject m_inner_subject;
};

Subject FrameBufferSizeHandler::m_inner_subject;