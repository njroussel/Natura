#pragma once

/* This kind of things make C++ a bad language. */
class Subject;

class Message {
public:
    Message(Subject *from){
        m_sender = from;
    }

    Subject *getSender(){
        return m_sender;
    }

private:
    Subject *m_sender;
};