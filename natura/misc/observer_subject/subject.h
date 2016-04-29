#pragma once

#include "observer.h"
#include "messages/message.h"
#include <vector>

class Subject{
public:
    virtual void attach(Observer *obs){
        m_observers.push_back(obs);
    }

    virtual void notify(Message *msg){
        for (Observer *obs : m_observers){
            obs->update(msg);
        }
    }

private:
    std::vector<Observer *> m_observers;
};