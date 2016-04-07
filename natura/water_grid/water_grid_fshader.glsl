#version 330

in vec2 uv;
in float height_;

out vec3 color;

uniform sampler1D colormap;

void main() {
    float value = (height_*1.5f + 1)/2.0f;
    color = texture(colormap, value).rgb;
}
