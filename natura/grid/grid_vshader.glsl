#version 330

in vec2 position;

out vec2 uv;

uniform mat4 MVP;
uniform sampler2D perlin_tex;

void main() {
    uv = position;

    float height = 0.25f * texture(perlin_tex, position).r;
    vec3 pos_3d = vec3(position.x, height, -position.y);
    gl_Position = MVP * vec4(pos_3d, 1.0);
}

