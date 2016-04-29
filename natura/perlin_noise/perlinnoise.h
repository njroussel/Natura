#pragma once

#include <cstdint>
#include <GL/glew.h>
#include "../perlin_quad/perlin_quad.h"
#include "../framebuffer.h"
#include "../misc/observer_subject/subject.h"
#include "../misc/observer_subject/messages/perlin_noise_prop_changed_message.h"

enum class PerlinNoiseProperty {H, LACUNARITY, OFFSET, FREQUENCY, OCTAVE};

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

    void setProperty(PerlinNoiseProperty prop, float value){
        switch (prop) {
            case PerlinNoiseProperty::H :
                m_H = value;
                break;

            case PerlinNoiseProperty::LACUNARITY :
                m_lacunarity = value;
                break;

            case PerlinNoiseProperty::OFFSET :
                m_offset = value;
                break;

            case PerlinNoiseProperty::FREQUENCY :
                m_frequency = value;
                break;

            case PerlinNoiseProperty::OCTAVE :
                m_octaves = static_cast<int>(value);
                break;
        }
        /* Notify the chunks. */
        notify(PerlinNoisePropChangedMessage());
    }

    float getProperty(PerlinNoiseProperty prop){
        switch (prop) {
            case PerlinNoiseProperty::H :
                return m_H;

            case PerlinNoiseProperty::LACUNARITY :
                return m_lacunarity;

            case PerlinNoiseProperty::OFFSET :
                return m_offset;

            case PerlinNoiseProperty::FREQUENCY :
                return m_frequency;

            case PerlinNoiseProperty::OCTAVE :
                return m_octaves;
        }
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


