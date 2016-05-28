#pragma once

#include "message.h"

class Ball;

class BallOutOfBoundsMessage : public Message{
public:
    BallOutOfBoundsMessage(Ball *ball) {
        m_type = Message::Type:: BALL_OUT_OF_BOUNDS;
        m_ball = ball;
    }

    Ball *getBallInstance() {
        return m_ball;
    }

private:
    Ball *m_ball;
};