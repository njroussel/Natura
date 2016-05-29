#version 330
#define noise_size 4.0f
#define min_fog_distance 30.0f
#define max_fog_distance 40.0f

in vec2 uv;
in vec3 light_dir;
in mat4 MV;
in float distance_camera;
in vec4 shadow_coord;

out vec4 out_color;
uniform vec2 quad_indices;
uniform sampler2D perlin_tex;
uniform sampler2D grass_tex;
uniform sampler2D rock_tex;
uniform sampler2D snow_tex;
uniform sampler2D sand_tex;
uniform sampler2D water_tex;
uniform mat4 model;


/* Shadows */
uniform float bias;
uniform sampler2D shadow_map;
uniform bool show_shadow;
uniform bool do_pcf;
uniform vec3 sun_light_dir;
uniform bool use_color;  // Use predefined color or texture?


uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

// Poisson disk sample locations.
vec2 poisson_disk[16] = vec2[](
   vec2(-0.94201624, -0.39906216),
   vec2(0.94558609, -0.76890725),
   vec2(-0.094184101, -0.92938870),
   vec2(0.34495938, 0.29387760),
   vec2(-0.91588581, 0.45771432),
   vec2(-0.81544232, -0.87912464),
   vec2(-0.38277543, 0.27676845),
   vec2(0.97484398, 0.75648379),
   vec2(0.44323325, -0.97511554),
   vec2(0.53742981, -0.47373420),
   vec2(-0.26496911, -0.41893023),
   vec2(0.79197514, 0.19090188),
   vec2(-0.24188840, 0.99706507),
   vec2(-0.81409955, 0.91437590),
   vec2(0.19984126, 0.78641367),
   vec2(0.14383161, -0.14100790)
);

float getPercentage( float value,  float min,  float max ){
    value = clamp( value, min, max );
    return (value - min) / (max - min);
}


void main() {
    vec2 pos_2d = uv;
    vec3 color;

    float height = ((texture(perlin_tex, pos_2d).r) + 1.0f) / 2.0f;
    vec3 grassColor = texture(grass_tex, pos_2d* 10.f).rgb;
    vec3 rockColor = texture(rock_tex, pos_2d* 2.0f).rgb;
    vec3 snowColor = texture(snow_tex, pos_2d* 5).rgb;
    vec3 sandColor = texture(sand_tex, pos_2d* 5).rgb;
    vec3 waterColor = texture(water_tex, pos_2d* 5).rgb;

    if(height > 0.9){
        color = snowColor;
    }
    else if (height > 0.85){
        color = mix(rockColor, snowColor, getPercentage(height, 0.85f, 0.9f));
    }
    else if (height > 0.65){
        color = mix(grassColor, rockColor, getPercentage(height, 0.65f, 0.85f));
    }
    else if( height > 0.5f){
        color = mix(sandColor, grassColor, getPercentage(height, 0.5f, 0.65f));
    }
    else if (height > 0.35f){
        color = mix(waterColor, sandColor, getPercentage(height, 0.35f, 0.5f));
    }
    else{
        color = waterColor;
    }

    float epsilon = 0.005f;
    float zDiffXaxis = texture(perlin_tex, vec2(pos_2d.x + epsilon, pos_2d.y)).r -
                        texture(perlin_tex, vec2(pos_2d.x - epsilon, pos_2d.y)).r;
    float zDiffYaxis = texture(perlin_tex, vec2(pos_2d.x, pos_2d.y + epsilon)).r -
                        texture(perlin_tex, vec2(pos_2d.x, pos_2d.y - epsilon)).r;

    vec3 normal = normalize(cross(vec3(2 *epsilon, zDiffXaxis, 0.0f), vec3(0.0, zDiffYaxis, 2* epsilon)));
    vec3 n = normal; // Normal in worlds coordinates.
    normal = (inverse(transpose(MV)) * vec4(-normal, 1.0f)).xyz;

    vec3 ambient = color * 0.6 * La;

    vec3 light = normalize(light_dir);
    float dotNl = dot(normal, light_dir) < 0.0f ? 0.0f : dot(normal, light_dir);
    vec3 diffuse = color * dotNl * Ld;

    vec3 r = 2 * normal * dotNl + light;
    r = normalize(r);
    float dotRv = dot(r, light) < 0.0f ? 0.0f : dot(r, light);

    vec3 specular = ks * pow(dotRv, alpha) * Ls;

    float fog_factor = (max_fog_distance - distance_camera) / (max_fog_distance - min_fog_distance);
    fog_factor = clamp(fog_factor, 0.0f, 1.0f);

    //color = mix(fog_colour, ambient + diffuse +specular, fog_factor);
    color = ambient + diffuse +specular;

    float ambient_light = 0.0;
        float shade = ambient_light + max(dot(normalize(n),
                                              normalize(sun_light_dir)), 0.0);

        // shading factor from the shadow (1.0 = no shadow, 0.0 = all dark)
        float shadow = 1.0;
        if (show_shadow) {
            // perspective division
            vec3 shadow_coord_norm = shadow_coord.xyz / shadow_coord.w;
            if (!do_pcf) {
                // Read only 1 shadow sample.
                if (texture(shadow_map, shadow_coord_norm.xy).r <
                    (shadow_coord_norm.z - bias)) {
                    shadow = 0.2;
                }
            } else {
                // Do percentage closer filtering with 16 samples
                for (int i = 0; i < 16; i++) {
                  if (texture(shadow_map, shadow_coord_norm.xy + poisson_disk[i]
                              / 200.0).r < (shadow_coord_norm.z - bias)) {
                    shadow -= 0.05;
                  }
                }
            }
        }

            color = shadow * shade * color.rgb;
            //color = vec3(shadow * shade);



    out_color.r = color.r;
    out_color.g = color.g;
    out_color.b = color.b;
    out_color.a = 1.0;
}
