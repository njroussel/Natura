#version 330
#define noise_size 4.0f

in vec2 position;

out vec2 uv;
uniform vec2 quad_indices;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform sampler2D perlin_tex;
uniform float amplitude;
uniform vec3 light_pos;

out vec3 light_dir;
out mat4 MV;

void main() {
    vec2 pos_2d = position;
    pos_2d.x += quad_indices.x;
    pos_2d.y += quad_indices.y;
    pos_2d = pos_2d / noise_size;
    float height = amplitude * (texture(perlin_tex, pos_2d).r - 0.5);
    vec3 pos_3d = vec3(position.x, height, position.y);

    MV = view * model;
    vec4 vpoint_mv = MV * vec4(pos_3d, 1.0);

    gl_Position = projection * vpoint_mv;
    //gl_Position = vec4(position.x, 0.0f, position.y, 1.0f);

    light_dir = -vec3(vpoint_mv);
    light_dir = normalize(light_dir);
    uv = pos_2d;
}

