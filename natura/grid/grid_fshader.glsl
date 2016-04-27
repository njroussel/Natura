#version 330

in vec2 uv;
in vec3 light_dir;
in mat4 MV;

out vec3 color;

uniform sampler2D perlin_tex;
uniform sampler1D colormap;
uniform vec3 La, Ld;
uniform vec3 ka, kd;


void main() {
    float componentColor = (texture(perlin_tex, uv).r + 0.5) / 2.0f;
    vec3 colormapValue = vec3(componentColor); //texture(colormap, componentColor).xyz;

    float epsilon = 0.01f;
    float zDiffXaxis = texture(perlin_tex, vec2(uv.x + epsilon, uv.y)).r -
                    texture(perlin_tex, vec2(uv.x - epsilon, uv.y)).r;
    float zDiffYaxis = texture(perlin_tex, vec2(uv.x, uv.y + epsilon)).r -
                    texture(perlin_tex, vec2(uv.x, uv.y - epsilon)).r;

    vec3 normal = normalize(cross(vec3(2 *epsilon, zDiffXaxis, 0.0f), vec3(0.0, zDiffYaxis, 2* epsilon)));
    normal = (inverse(transpose(MV)) * vec4(-normal, 1.0f)).xyz;;
    vec3 ambient = colormapValue * La;
    vec3 light = normalize(light_dir);
    float dotNl = dot(normal, light) < 0.0f ? 0.0f : dot(normal, light);
    vec3 diffuse = colormapValue * 0.5f * dotNl * Ld;

    color = diffuse + ambient;
}
