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

void main() {
    uv = position;
    float height = amplitude * (texture(perlin_tex, position).r - 0.5);
    vec3 pos_3d = vec3(position.x, height, position.y);

    mat4 MV = view * model;
    vec4 vpoint_mv = MV * vec4(pos_3d, 1.0);

    float zDiffXaxis = texture(perlin_tex, vec2(position.x + 0.01f, position.y)).r -
                texture(perlin_tex, vec2(position.x - 0.01f, position.y)).r;
    float zDiffYaxis = texture(perlin_tex, vec2(position.x, position.y + 0.01f)).r -
                texture(perlin_tex, vec2(position.x, position.y - 0.01f)).r;

    vec3 normal = normalize(cross(vec3(0.0f, 0.02f, zDiffXaxis), vec3(0.02f, 0.0f, zDiffYaxis)));

    normal_mv = (inverse(transpose(MV)) * vec4(normal, 1.0f)).xyz;
    normal_mv = normalize(normal_mv);

    light_dir = light_pos - vpoint_mv.xyz;
    light_dir = normalize(light_dir);

    gl_Position = projection * vpoint_mv;
}

