#version 330

in vec2 uv;
in vec3 light_dir;
in float height_;
in mat4 MV_out;
in vec3 view_dir;
in vec3 normal;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

out vec4 color;

uniform sampler1D colormap;
uniform sampler2D tex_reflection;

void main() {
    float epsilon = 0.005f;
    float value = height_;
    vec3 water_color = texture(colormap, value).rgb;

    vec3 ambient = water_color * 0.6 * La;

    vec3 normal_normalized = normalize(normal);
    vec3 light = normalize(light_dir);

    float dotNl = dot(normal_normalized, light) < 0.0f ? 0.0f : dot(normal_normalized, light);
    vec3 diffuse = water_color * dotNl * Ld;


    vec3 r = 2 * normal_normalized * dotNl + light;
    r = normalize(r);
    float dotRv = dot(r, light) < 0.0f ? 0.0f : dot(r, light);

    vec3 specular = ks * pow(dotRv, alpha) * Ls;

    //reflection
    vec2 window = textureSize(tex_reflection, 0).xy;
    float window_height = window.y;
    float window_width = window.x;
    vec4 window_coord = gl_FragCoord;
    float width_normed = window_coord.x / window_width;
    float height_normed = window_coord.y / window_height;
    vec2 new_uv = vec2(width_normed, 1 - height_normed);
    vec3 color_from_mirror = texture(tex_reflection, new_uv).rgb;

    vec3 original_color = specular + diffuse + ambient;
    color = vec4(mix(color_from_mirror, original_color, vec3(0.65f)), 0.6f);
}