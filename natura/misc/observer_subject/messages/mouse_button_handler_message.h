#pragma once

#include "message.h"

class MouseButtonHandlerMessage : public Message{
public:
    MouseButtonHandlerMessage(GLFWwindow *window, int button, int action, int mod){
        m_type = Message::Type::MOUSE_BUTTON_INPUT;
        m_window = window;
        m_button = button;
        m_action = action;
        m_mod = mod;
    }

    GLFWwindow *getWindow(){
        return m_window;
    }

    int getButton(){
        return m_button;
    }

    int getAction(){
        return m_action;
    }

    int getMod(){
        return m_mod;
    }

private:
    GLFWwindow *m_window;
    int m_button;
    int m_action;
    int m_mod;
};