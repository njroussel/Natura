#version 330

out vec3 color;
in vec3 uv;

void main() {
    color = ceil(uv);
}