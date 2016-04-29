#pragma once

#include "message.h"

class FrameBufferSizeHandlerMessage : public Message{
public:
    FrameBufferSizeHandlerMessage(GLFWwindow *window, int width, int height){
        m_type = Message::Type::FRAMEBUFFER_SIZE_CHANGE;
        m_window = window;
        m_width = width;
        m_height = height;
    }

    GLFWwindow *getWindow(){
        return m_window;
    }

    int getWidth(){
        return m_width;
    }

    int getHeight(){
        return m_height;
    }

private:
    GLFWwindow *m_window;
    int m_width;
    int m_height;
};