#pragma once

/* This kind of things make C++ a bad language. */
class Subject;

class Message {
public:
    enum class Type{EMPTY, PERLIN_PROP_CHANGE, KEYBOARD_HANDLER_INPUT};

    Message(){
        m_type = Message::Type::EMPTY;
    }

    Message::Type getType(){
        return m_type;
    }

protected:
    Message::Type m_type;
};