#version 330
#define noise_size 4.0f
#define min_fog_distance 20.0f
#define max_fog_distance 30.0f
#define fog_colour vec3(0.4, 0.4, 0.4)

in vec2 uv;
in vec3 light_dir;
in mat4 MV;
in float distance_camera;

out vec3 color;
uniform vec2 quad_indices;
uniform sampler2D perlin_tex;
uniform sampler2D grass_tex;
uniform sampler2D rock_tex;
uniform sampler2D snow_tex;
uniform sampler2D sand_tex;
uniform sampler2D water_tex;
uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

float getPercentage( float value,  float min,  float max ){
    value = clamp( value, min, max );
    return (value - min) / (max - min);
}


void main() {

    vec2 pos_2d = uv;

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

    color = mix(fog_colour, ambient + diffuse +specular, fog_factor);
}
