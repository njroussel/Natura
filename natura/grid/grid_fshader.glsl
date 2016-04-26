#version 330

in vec2 uv;
in vec3 normal_mv;
in vec3 light_dir;

out vec3 color;

uniform sampler2D perlin_tex;
uniform sampler1D colormap;
uniform vec3 La, Ld;
uniform vec3 ka, kd;

void main() {
    float componentColor = (texture(perlin_tex, uv/2).r + 0.5) / 2.0f;
    vec3 colormapValue = texture(colormap, componentColor).xyz;

    vec3 ambient = colormapValue * La;
    vec3 normal = normalize(normal_mv);
    vec3 light = normalize(light_dir);
    float dotNl = dot(normal, light) < 0.0f ? 0.0f : dot(normal, light);
    vec3 diffuse = colormapValue * 0.5f * dotNl * Ld;

    color = diffuse + ambient;
}
