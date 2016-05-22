#version 330
#define PI radians(180.0f)

in vec2 position;

out vec2 uv;
out float height_;

out vec3 light_dir;
out mat4 MV_out;
out vec3 view_dir;
out vec3 normal;

uniform mat4 MV;
uniform mat4 projection;
uniform float time;
uniform vec2 chunk_pos;
uniform vec3 light_pos;

int component_count = 8;
float wavelength[8];
float speed[8];
float amplitude[8];

void fill_params(){
    wavelength[0] = 1;
    for (int i = 1 ; i < component_count ; i ++){
       wavelength[i] = wavelength[i-1] / 2;
    }
    speed[0] = 1.f;
    for (int i = 1 ; i < component_count ; i ++){
       speed[i] = speed[i-1] / 2.f;
    }
    amplitude[0] = 1.f / 80.f;
    for (int i = 1 ; i < component_count ; i ++){
       amplitude[i] = amplitude[i-1] / 2.f;
    }
}

float wave_h(float x, float y) {
    float height = 0.0;
    for (int i = 0; i < component_count; ++i){
        float freq_factor = dot(vec2(cos(i * PI / 2.f), sin(i * PI / 2.f)), vec2(x, y));
        height += amplitude[i] * sin((2*PI/wavelength[i]) * (freq_factor + time * speed[i]));
    }
    return height;
}
void main() {
    fill_params();
    vec2 pos_2d = position;
    pos_2d.x += chunk_pos.x;
    pos_2d.y += chunk_pos.y;
    float height = wave_h(pos_2d.x, pos_2d.y);
    vec3 pos_3d = vec3(position.x, height, position.y);

    uv = pos_2d;

    height_ = height;

    float epsilon = 0.01f;
    float zDiffXaxis = wave_h(pos_2d.x + epsilon, pos_2d.y) - wave_h(pos_2d.x - epsilon, pos_2d.y);
    float zDiffYaxis = wave_h(pos_2d.x, pos_2d.y + epsilon) - wave_h(pos_2d.x, pos_2d.y - epsilon);

    normal = -normalize((cross(vec3(2 * epsilon, zDiffXaxis, 0.0f), vec3(0.0, zDiffYaxis, 2* epsilon))));

    vec4 vpoint_mv = MV * vec4(pos_3d, 1.0);
    gl_Position = projection * vpoint_mv;

    MV_out = MV;

    light_dir = -vec3(vpoint_mv);
}
