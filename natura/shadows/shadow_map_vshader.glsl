#version 330
#define noise_size 4.0f

in vec2 position;

out vec2 uv;
uniform vec2 quad_indices;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform float amplitude;
uniform float water_height;
uniform vec3 light_pos;
uniform vec3 cam_pos;
uniform vec2 chunk_pos;
uniform int terrain_size; /* Size of terrain in chunks. */

uniform sampler2D perlin_tex;
uniform sampler2D left_tex;
uniform bool left_present;
uniform sampler2D low_tex;
uniform bool low_present;
uniform sampler2D low_left_tex;
uniform bool low_left_present;
uniform mat4 depth_vp_offset;


uniform mat4 depth_vp;
out vec3 light_dir;
out float distance_camera;
out mat4 MV;
out vec4 shadow_coord;

/* Sampler2D are opaque types so this function is handy to avoid duplication. */
float getTextureVal(vec2 pos){
    if (chunk_pos.x == 0 && pos.x == 0){
        /* This is actually a trick to prevent the light coming 'under' the terrain.
        */
            return -100.0;
    }
    else if (chunk_pos.y == 0 && pos.y == 0){
        return -100.0;
    }
    else if (chunk_pos.x == terrain_size-1 && pos.x == 1.0){
            return -100.0;
        }
    else if (chunk_pos.y == terrain_size-1 && pos.y == 1.0){
        return -100.0;
    }
    else {
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

    gl_Position = depth_vp * model * vec4(pos_3d, 1.0);

}

