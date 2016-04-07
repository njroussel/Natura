#version 330

in vec2 position;

out vec2 uv;

uniform mat4 MVP;
uniform sampler2D perlin_tex;

void main() {
    uv = position;

    float height = length(texture(perlin_tex, position).r) / sqrt(255.0f);
    vec3 pos_3d = vec3(position.x, 0, -position.y);
    gl_Position = MVP * vec4(pos_3d, 1.0);
}

