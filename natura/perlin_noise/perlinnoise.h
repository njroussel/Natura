#pragma once

#include <cstdint>

class PerlinNoise {

public:
    PerlinNoise(uint32_t width, uint32_t height){
        mWidth = width;
        mHeight = height;
    }

    int generateNoise(){
        quad.Init();
        int tex = frameBuffer.Init(mWidth, mHeight);
        frameBuffer.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        quad.Draw(IDENTITY_MATRIX);
        frameBuffer.Unbind();
        return tex;
    }

    void Cleanup(){
        frameBuffer.Cleanup();
        quad.Cleanup();
    }

    void Draw(mat4 MVP){
        quad.Draw(MVP);
    }

private:
    uint32_t mWidth;
    uint32_t mHeight;
    FrameBuffer frameBuffer;
    PerlinQuad quad;
};


