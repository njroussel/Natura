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

uniform sampler2D perlin_tex;
uniform sampler2D left_tex;
uniform bool left_present;
uniform sampler2D low_tex;
uniform bool low_present;
uniform sampler2D low_left_tex;
uniform bool low_left_present;
uniform mat4 depth_vp_offset;


out vec3 light_dir;
out float distance_camera;
out mat4 MV;
out vec4 shadow_coord;

/* Sampler2D are opaque types so this function is handy to avoid duplication. */
float getTextureVal(vec2 pos){
    if (pos.x >= 1.0f && pos.y >= 1.0 && low_left_present){
        return texture(low_left_tex, vec2(0.0, 0.0)).r;
    }
    else if(pos.x >= 1.0f && low_present){
        return texture(low_tex, vec2(0.0f, pos.y)).r;
    }
    else if (pos.y >= 1.0f && left_present){
        return texture(left_tex, vec2(pos.x, 0.0f)).r;
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
    shadow_coord = depth_vp_offset * model * vec4(pos_3d, 1.0);
    MV = view * model;
    vec4 vpoint_mv = MV * vec4(pos_3d, 1.0);
    distance_camera = length(vpoint_mv);

    gl_Position = projection * vpoint_mv;

    gl_ClipDistance[0] = height - water_height;

    light_dir = -vec3(vpoint_mv);
    light_dir = normalize(light_dir);
    uv = pos_2d;
}

