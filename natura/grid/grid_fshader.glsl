#version 330

in vec2 uv;
in vec3 normal_mv;
in vec3 light_dir;

out vec3 color;

uniform sampler2D perlin_tex;
uniform vec3 La, Ld;
uniform vec3 ka, kd;

void main() {
    vec3 ambient = ka * La;
    vec3 normal = normalize(normal_mv);
    float dotNl = dot(normal, light_dir) < 0.0f ? 0.0f : dot(normal, light_dir);
    vec3 diffuse = kd * dotNl * Ld;

    float componentColor = (texture(perlin_tex, uv).r + 1) / 2.0f;
    color = vec3(componentColor);
    color = diffuse + ambient;
}
