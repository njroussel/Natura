#pragma once

#include "../../../observer_subject/messages/message.h"
#include "../../../observer_subject/observer.h"
#include "../../../observer_subject/subject.h"

class Handler : public Subject{
public:
    void attach(Observer *obs){
        m_inner_subject.attach(obs);
    }

    void notify(Message *msg){
        m_inner_subject.notify(msg);
    }

    /* Virtual destructor so that the class is abstract. */
    virtual ~Handler() = 0;

protected :
    static Subject m_inner_subject;
};

Subject Handler::m_inner_subject;

Handler::~Handler() {}