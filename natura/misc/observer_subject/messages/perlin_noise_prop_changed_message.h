#pragma once

#include "message.h"

/* Looks like a Java class name. */
class PerlinNoisePropChangedMessage : public Message{
public:
    PerlinNoisePropChangedMessage() : Message() {
        m_type = Message::Type::PERLIN_PROP_CHANGE;
    }
};