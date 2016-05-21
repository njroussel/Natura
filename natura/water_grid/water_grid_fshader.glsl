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



    vec2 window = textureSize(tex_reflection, 0).xy;
    /// TODO: query window_width/height using the textureSize(..) function on tex_mirror
    float window_height = window.y;
    float window_width = window.x;

        /// TODO: use gl_FragCoord to build a new [_u,_v] coordinate to query the framebufferFloor
        /// NOTE: make sure you normalize gl_FragCoord by window_width/height
        /// NOTE: you will have to flip the "v" coordinate as framebufferFloor is upside/down
    vec4 window_coord = gl_FragCoord;
    float width_normed = window_coord.x / window_width;
    float height_normed = window_coord.y / window_height;
    vec2 new_uv = vec2(width_normed, 1 - height_normed);

        /// TODO: mix the texture(tex,uv).rgb with the value you fetch by texture(tex_mirror,vec2(_u,_v)).rgb
    vec3 color_from_mirror = texture(tex_reflection, new_uv).rgb;
    color = vec4(mix(color_from_mirror, color_, vec3(.55)), 0.8f);

}