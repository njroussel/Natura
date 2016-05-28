#pragma once

#include "../subject.h"

/* This kind of things (forward decl.) make C++ a bad language. */
class Subject;

class Message {
public:
    enum class Type{EMPTY, PERLIN_PROP_CHANGE, KEYBOARD_HANDLER_INPUT, MOUSE_BUTTON_INPUT, MOUSE_CURSOR_INPUT, FRAMEBUFFER_SIZE_CHANGE, BALL_OUT_OF_BOUNDS};

    Message(){
        m_type = Message::Type::EMPTY;
    }

    Message::Type getType(){
        return m_type;
    }

protected:
    Message::Type m_type;
};