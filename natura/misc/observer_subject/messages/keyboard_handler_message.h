#pragma once

#include "message.h"

class KeyboardHandlerMessage : public Message{

public:
    KeyboardHandlerMessage(GLFWwindow *window, int key, int scancode, int action, int mods) : Message() {
        m_type = Message::Type::KEYBOARD_HANDLER_INPUT;
        m_window = window;
        m_key = key;
        m_scancode = scancode;
        m_action = action;
        m_mods = mods;
    }

    GLFWwindow *getWindow(){
        return m_window;
    }

    int getKey(){
        return m_key;
    }

    int getScancode(){
        return m_scancode;
    }

    int getAction(){
        return m_action;
    }

    int getMods(){
        return m_mods;
    }

private:
    GLFWwindow *m_window;
    int m_key;
    int m_scancode;
    int m_action;
    int m_mods;
};