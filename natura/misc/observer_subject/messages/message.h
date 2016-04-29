#pragma once

/* This kind of things make C++ a bad language. */
class Subject;

class Message {
public:
    enum class Type{EMPTY, PERLIN_PROP_CHANGE};

    Message(Subject *from, Message::Type type = Message::Type::EMPTY){
        m_type = type;
        m_sender = from;
    }

    Subject *getSender(){
        return m_sender;
    }

    Message::Type getType(){
        return m_type;
    }

private:
    Subject *m_sender;
    Message::Type m_type;
};