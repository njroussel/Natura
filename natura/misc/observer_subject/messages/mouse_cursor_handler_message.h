#pragma once

#include "message.h"

class MouseCursorHandlerMessage : public Message{
public:
    MouseCursorHandlerMessage(GLFWwindow *window, double x, double y){
        m_type = Message::Type::MOUSE_CURSOR_INPUT;
        m_window = window;
        m_x = x;
        m_y = y;
    }

    GLFWwindow *getWindow(){
        return m_window;
    }

    double getCoordX(){
        return m_x;
    }

    double getCoordY(){
        return m_y;
    }

private:
    GLFWwindow *m_window;
    double m_x;
    double m_y;
};