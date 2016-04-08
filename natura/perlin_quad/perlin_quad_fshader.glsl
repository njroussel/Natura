#version 330 core

in vec2 uv;

out vec3 color;

void main() {
    color = vec3(length(uv), 0.0f, 0.0f);
}
