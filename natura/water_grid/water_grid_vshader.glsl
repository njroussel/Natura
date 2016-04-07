#version 330
#define PI radians(180)

in vec2 position;

out vec2 uv;
out float height_;

uniform mat4 MVP;
uniform float time;

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
    amplitude[0] = 1.f / 20.f;
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
    uv = (position + vec2(1.0, 1.0)) * 0.5;

    // convert the 2D position into 3D positions that all lay in a horizontal
    // plane.
    // TODO 6: animate the height of the grid points as a sine function of the
    // 'time' and the position ('uv') within the grid.
    // Water
    float height = wave_h(uv[0], uv[1]);
    height_ = height;
    vec3 pos_3d = vec3(position.x, height, -position.y);
    gl_Position = MVP * vec4(pos_3d, 1.0);
}

