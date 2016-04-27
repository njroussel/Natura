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

    int generateNoise(float H, float frequency, float lacunarity, float offset, int octaves) {
        quad.Init();
        int tex = frameBuffer.Init(mWidth, mHeight);
        frameBuffer.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        quad.Draw(IDENTITY_MATRIX, H, frequency, lacunarity, offset, octaves, glm::vec2(0.0, 0.0));
        frameBuffer.Unbind();
        return tex;
    }

    int refreshNoise(uint32_t width, uint32_t height, float H, float frequency, float lacunarity, float offset, int octaves, glm::vec2 displ) {
        mWidth = width;
        mHeight = height;
        frameBuffer.Cleanup();
        int tex = frameBuffer.Init(width, height);

        frameBuffer.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        quad.Draw(IDENTITY_MATRIX, H, frequency, lacunarity, offset, octaves, displ);
        frameBuffer.Unbind();
        return tex;
    }

    void Cleanup() {
        frameBuffer.Cleanup();
        quad.Cleanup();
    }

    void Draw(float H){
       // quad.Draw(IDENTITY_MATRIX, H, );
    }
    

private:
    uint32_t mWidth;
    uint32_t mHeight;
    FrameBuffer frameBuffer;
    PerlinQuad quad;
};


