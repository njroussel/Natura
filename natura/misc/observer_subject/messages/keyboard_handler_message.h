#pragma once

#include "message.h"

class KeyboardHandlerMessage : public Message{

public:
    KeyboardHandlerMessage(int key, int scancode, int action, int mods) : Message() {
        m_type = Message::Type::KEYBOARD_HANDLER_INPUT;
        m_key = key;
        m_scancode = scancode;
        m_action = action;
        m_mods = mods;
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
    int m_key;
    int m_scancode;
    int m_action;
    int m_mods;
};