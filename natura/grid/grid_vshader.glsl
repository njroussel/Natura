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

uniform sampler2D left_tex;
uniform sampler2D low_tex;

out vec3 light_dir;
out mat4 MV;

/* Sampler2D are opaque types so this function is handy to avoid duplication. */
float getTextureVal(vec2 pos){
    if (pos.x >= 1.0f){
        pos.x = 0.0f;
        return texture(left_tex, pos).r;
    }
    else if (pos.y >= 1.0f){
        pos.y = 0.0f;
        return texture(low_tex, pos).r;
    }
    else{
        return texture(perlin_tex, pos).r;
    }
}

void main() {
    vec2 pos_2d = position;
    pos_2d.x += quad_indices.x;
    pos_2d.y += quad_indices.y;
    pos_2d = pos_2d / noise_size;
    float height = amplitude * (getTextureVal(pos_2d) - 0.5);
    vec3 pos_3d = vec3(position.x, height, position.y);

    MV = view * model;
    vec4 vpoint_mv = MV * vec4(pos_3d, 1.0);

    gl_Position = projection * vpoint_mv;

    gl_ClipDistance[0] = height;

    light_dir = -vec3(vpoint_mv);
    light_dir = normalize(light_dir);
    uv = pos_2d;
}

