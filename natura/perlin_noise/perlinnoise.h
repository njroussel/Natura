#pragma once

#include <cstdint>
#include <GL/glew.h>
#include <bits/stl_deque.h>
#include "../perlin_quad/perlin_quad.h"
#include "../framebuffer.h"
#include "../misc/observer_subject/subject.h"
#include "../misc/observer_subject/messages/perlin_noise_prop_changed_message.h"

enum class PerlinNoiseProperty {H, LACUNARITY, OFFSET, FREQUENCY, OCTAVE};

class PerlinNoise : public Subject{
public:
    PerlinNoise(uint32_t width, uint32_t height, glm::vec2 cache_size) {
        mWidth = width;
        mHeight = height;

        for (int i = 0 ; i < cache_size.y ; i ++){
            m_frame_buffers.push_back(std::deque<FrameBuffer> (static_cast<unsigned long> (cache_size.x), FrameBuffer()));
        }
        m_terrain_offset = glm::vec2(0,0);
        m_cache_size = cache_size;
    }

    void Init(){
        for (int i = 0 ; i < m_frame_buffers.size() ; i ++){
            for (int j = 0 ; j < m_frame_buffers[i].size() ; j ++){
                m_frame_buffers[i][j].Init(mWidth, mHeight);
            }
        }
        quad.Init();
    }

    int generateNoise(glm::vec2 displ) {
        glm::vec2 id = displ - m_terrain_offset;
        FrameBuffer frameBuffer = m_frame_buffers[(int)id.y][(int)id.x];
        cout << "generateNoise : " << endl;
        cout << "displ  = " << displ.x << " " << displ.y << endl;
        cout << "id = " << id.x << " " << id.y << endl;
        int tex = frameBuffer.getTextureId();

        frameBuffer.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        quad.Draw(IDENTITY_MATRIX, m_H, m_frequency, m_lacunarity, m_offset, m_octaves, displ);
        frameBuffer.Unbind();
        //frameBuffer.Cleanup();
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
        Message *m = new PerlinNoisePropChangedMessage();
        notify(m);
        delete m;
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

    void setTerrainOffset(glm::vec2 offset){
        glm::vec2 dir = m_terrain_offset - offset;
        m_terrain_offset = offset;
        if (dir.y > 0){
            cout << "< 0" << endl;
            m_frame_buffers.push_front(m_frame_buffers[m_frame_buffers.size()-1]);
            m_frame_buffers.pop_back();
        }
        else if (dir.y < 0){
            cout << "> 0" << endl;
            m_frame_buffers.push_back(m_frame_buffers[0]);
            m_frame_buffers.pop_front();
        }
        else if (dir.x < 0){
            for (int i = 0 ; i < m_frame_buffers.size() ; i ++){
                m_frame_buffers[i].push_back(m_frame_buffers[i][0]);
                m_frame_buffers[i].pop_front();
            }
        }
        else if (dir.x > 0){
            for (int i = 0 ; i < m_frame_buffers.size() ; i ++){
                m_frame_buffers[i].push_front(m_frame_buffers[i][m_frame_buffers.size()-1]);
                m_frame_buffers[i].pop_back();
            }
        }
    }

    void Cleanup() {
        quad.Cleanup();
        for (int i = 0 ; i < m_frame_buffers.size() ; i ++){
            for (int j = 0 ; j < m_frame_buffers[i].size() ; j ++){
                m_frame_buffers[i][j].Cleanup();
            }
        }
    }

    FrameBuffer *getFrameBufferForChunk(glm::vec2 chunkpos){
        return &m_frame_buffers[(int)chunkpos.y][(int)chunkpos.x];
    }

private:
    std::deque<std::deque<FrameBuffer> > m_frame_buffers;
    glm::vec2 m_terrain_offset;
    glm::vec2 m_cache_size;
    uint32_t mWidth;
    uint32_t mHeight;
    PerlinQuad quad;
    float m_H = 0.35f;
    float m_lacunarity = 2.5f;
    float m_offset = 0.2f;
    float m_frequency = 0.1f;
    int m_octaves = 6;
};