#version 330
#define min_fog_distance 30.0f
#define max_fog_distance 40.0f

in vec3 normal_mv;
in vec3 view_dir;
in vec3 light_dir;
in float distance_camera;


out vec4 color;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;


void main() {
    vec3 ambient = ka * La;

    float dotNl = dot(normal_mv, light_dir) < 0.0f ? 0.0f : dot(normal_mv, light_dir);
    vec3 diffuse = kd * dotNl * Ld;

    vec3 r = 2 * normal_mv * dotNl - light_dir;
    r = normalize(r);
    float dotRv = dot(r, view_dir) < 0.0f ? 0.0f : dot(r, view_dir);

    float fog_factor = (max_fog_distance - distance_camera) / (max_fog_distance - min_fog_distance);
    fog_factor = clamp(fog_factor, 0.0f, 1.0f);

    vec3 specular = ks * pow(dotRv, alpha) * Ls;
    color = vec4(ambient + diffuse + specular, fog_factor);
}
