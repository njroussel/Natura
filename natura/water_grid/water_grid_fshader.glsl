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

void main() {
float epsilon = 0.005f;
    float value = (height_*1.5f + 1)/2.0f;
    vec3 color_;

    color_ = texture(colormap, value).rgb;

    vec3 ambient = color_ * 0.6 * La;
    vec3 light = normalize(light_dir);
    float dotNl = dot(normal, light) < 0.0f ? 0.0f : dot(normal, light);
    vec3 diffuse = color_ * dotNl * Ld;
    vec3 r = 2 * normal * dotNl - light_dir;
        r = normalize(r);
        float dotRv = dot(r, view_dir) < 0.0f ? 0.0f : dot(r, view_dir);
        vec3 specular = ks * pow(dotRv, alpha) * Ls;
        vec3 apd = diffuse + ambient + specular;
    color = vec4(apd.x, apd.y, apd.z, 0.6);
   // color = vec4(Ls.x, Ls.y, Ls.z, 1.f);
    //color = vec4(1-dotNl, 1-dotNl, 1-dotNl, 1.f);
    /*if (dotRv == 0)
        color = vec4(1.f, 0, 0, 1.f);
    else
        color = vec4(0.f, 1, 0, 1.f);*/
}