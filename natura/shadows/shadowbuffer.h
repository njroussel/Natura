#pragma once
#include "icg_helper.h"

class ShadowBuffer {

    private:
        bool init_;
        int width_;
        int height_;
        GLuint frame_buffer_object_;
        GLuint depth_texture_;
        GLint previous_viewport_[4];

    public:
        ShadowBuffer(int image_width = 4096, int image_height = 4096){
            this->width_ = image_width;
            this->height_ = image_height;
        }
        
        // warning: overrides viewport !!
        void Bind() {
            // Store the previous viewport
            glGetIntegerv(GL_VIEWPORT, previous_viewport_);
            glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_object_);
            glViewport(0, 0, width_, height_);
        }

        void Unbind() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            // Reset the viewport
            glViewport(previous_viewport_[0], previous_viewport_[1],
                    previous_viewport_[2], previous_viewport_[3]);
        }

        // TODO: Analyze this function to understand how to create an FBO
        //       with only a depth attachment and without writing to any
        //       color buffer.
        int Init() {
            // create color attachment
            {
                glActiveTexture(GL_TEXTURE1);
                glGenTextures(1, &depth_texture_);
                glBindTexture(GL_TEXTURE_2D, depth_texture_);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width_,
                             height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            }

            // tie it all together
            {
                glGenFramebuffers(1, &frame_buffer_object_);
                glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_object_);
                glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                     depth_texture_, 0);

                if (glCheckFramebufferStatus(GL_FRAMEBUFFER)
                    != GL_FRAMEBUFFER_COMPLETE)
                    std::cerr << "!!!ERROR: Framebuffer not OK :(" << std::endl;

                // TODO: Note that writing to any color is disabled,
                //       only depth is written.
                glDrawBuffer(GL_NONE);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
            return depth_texture_;
        }

        void Cleanup() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0 /*UNBIND*/);
            glDeleteFramebuffers(1, &frame_buffer_object_);
            glDeleteTextures(1, &depth_texture_);
        }
};
