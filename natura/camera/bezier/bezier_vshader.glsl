#version 330

in vec3 position;
uniform mat4 MVP;

out vec3 uv;

void main() {
    gl_Position = MVP * vec4(position*2, 1.0f);
    uv = position;
}
