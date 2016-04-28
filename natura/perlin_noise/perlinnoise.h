#pragma once

#include <cstdint>
#include "../perlin_quad/perlin_quad.h"
#include "../framebuffer.h"
#include "../misc/observer_subject/subject.h"

enum class PerlinProperty {H, LACUNARITY, OFFSET, FREQUENCY, OCTAVE};

class PerlinNoise : public Subject{
public:
    PerlinNoise(uint32_t width, uint32_t height) {
        mWidth = width;
        mHeight = height;
    }

    int generateNoise(glm::vec2 displ) {
        quad.Init();
        FrameBuffer frameBuffer;
        int tex = frameBuffer.Init(mWidth, mHeight);
        frameBuffer.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        quad.Draw(IDENTITY_MATRIX, m_H, m_frequency, m_lacunarity, m_offset, m_octaves, displ);
        frameBuffer.Unbind();
        frameBuffer.Cleanup();
        return tex;
    }

    void setProperty(PerlinProperty prop, float value){
        switch (prop) {
            case PerlinProperty::H :
                m_H = value;
                break;

            case PerlinProperty::LACUNARITY :
                m_lacunarity = value;
                break;

            case PerlinProperty::OFFSET :
                m_offset = value;
                break;

            case PerlinProperty::FREQUENCY :
                m_frequency = value;
                break;

            case PerlinProperty::OCTAVE :
                m_octaves = static_cast<int>(value);
                break;
        }
        /* Notify the chunks. */
        notify();
    }

    void Cleanup() {
        quad.Cleanup();
    }

private:
    uint32_t mWidth;
    uint32_t mHeight;
    PerlinQuad quad;
    float m_H = 0.35f;
    float m_lacunarity = 2.4f;
    float m_offset = -0.2f;
    float m_frequency = 2.04f;
    int m_octaves = 8;
};


