#version 330
#define PI radians(180)

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
    float epsilon = 0.005f;
    uv = (position + vec2(1.0, 1.0) + chunk_pos/4) * 0.5;

    // convert the 2D position into 3D positions that all lay in a horizontal
    // plane.
    // TODO 6: animate the height of the grid points as a sine function of the
    // 'time' and the position ('uv') within the grid.
    // Water
    float height = wave_h(uv[0]/* + chunk_pos.x*/, uv[1] /*s+ chunk_pos.y*/);
    height_ = height;
    vec3 pos_3d = vec3(position.x, height, position.y);

     float zDiffXAxis = wave_h(uv.x + epsilon, uv.y) - wave_h(uv.x - epsilon, uv.y);
     float zDiffYAxis = wave_h(uv.x, uv.y + epsilon) - wave_h(uv.x, uv.y - epsilon);
    normal = normalize(cross(vec3(2 *epsilon, zDiffXAxis, 0.0f), vec3(0.0, zDiffYAxis, 2* epsilon)));
normal = (inverse(transpose(MV)) * vec4(-normal, 1.0f)).xyz;


    vec4 vpoint_mv = MV * vec4(pos_3d, 1.0);
    gl_Position = projection * vpoint_mv;

    light_dir = light_pos - vpoint_mv.xyz;
        light_dir = normalize(light_dir);

        view_dir = -vpoint_mv.xyz;
            view_dir  = normalize(view_dir);

        MV_out = MV;
}
