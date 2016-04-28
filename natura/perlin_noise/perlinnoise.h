#pragma once

#include <cstdint>
#include "../perlin_quad/perlin_quad.h"
#include "../framebuffer.h"

class PerlinNoise {

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
        quad.Draw(IDENTITY_MATRIX, H, frequency, lacunarity, offset, octaves, displ);
        frameBuffer.Unbind();
        frameBuffer.Cleanup();
        return tex;
    }

    void Cleanup() {
        quad.Cleanup();
    }

    void Draw(float H){
       // quad.Draw(IDENTITY_MATRIX, H, );
    }
    

private:
    uint32_t mWidth;
    uint32_t mHeight;
    PerlinQuad quad;
    float H = 0.35f;
    float lacunarity = 2.4f;
    float offset = -0.2f;
    float frequency = 2.04f;
    int octaves = 8;
};


