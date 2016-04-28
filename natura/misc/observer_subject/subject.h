#pragma once

#include "observer.h"
#include <vector>

class Subject{
public:
    void attach(Observer *obs){
        m_observers.push_back(obs);
    }

    virtual void notify(){
        for (Observer *obs : m_observers){
            obs->update();
        }
    }

private:
    std::vector<Observer *> m_observers;
};