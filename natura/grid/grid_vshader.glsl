#version 330

in vec2 position;

out vec2 uv;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform sampler2D perlin_tex;
uniform float amplitude;
uniform vec3 light_pos;

out vec3 light_dir;
out vec3 normal_mv;
out mat4 MV;

void main() {
    uv = position;
    float height = amplitude * (texture(perlin_tex, position).r - 0.5);
    vec3 pos_3d = vec3(position.x, height, position.y);

    MV = view * model;
    vec4 vpoint_mv = MV * vec4(pos_3d, 1.0);

    gl_Position = projection * vpoint_mv;

    light_dir = -vec3(vpoint_mv);
    light_dir = normalize(light_dir);
}

