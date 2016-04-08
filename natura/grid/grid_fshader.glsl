#version 330

in vec2 uv;

out vec3 color;

uniform sampler2D tex;

void main() {
    float componentColor = (1 + texture(tex, uv).r)/ 2.0f;
    color = vec3(componentColor);
}
