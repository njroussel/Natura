#pragma once

#include "messages/message.h"

class Observer {
public:
    virtual void update(Message &msg) = 0;
};