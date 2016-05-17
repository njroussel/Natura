#version 330
#define noise_size 4.0f

in vec2 uv;
in vec3 light_dir;
in mat4 MV;

out vec3 color;
uniform vec2 quad_indices;
uniform sampler2D perlin_tex;
uniform sampler2D grass_tex;
uniform sampler2D rock_tex;
uniform sampler2D snow_tex;
uniform sampler2D sand_tex;
uniform sampler2D water_tex;
uniform vec3 La, Ld;
uniform vec3 ka, kd;

float getPercentage( float value,  float min,  float max ){
    value = clamp( value, min, max );
    return (value - min) / (max - min);
}


void main() {

    vec2 pos_2d = uv;
    //pos_2d.x += quad_indices.x;
    //pos_2d.y += quad_indices.y;
    //spos_2d = pos_2d / noise_size;


    float height = ((texture(perlin_tex, pos_2d).r) + 1.0f) / 2.0f;
    vec3 grassColor = texture(grass_tex, pos_2d* 30.f).rgb;
    vec3 rockColor = texture(rock_tex, pos_2d* 10.0f).rgb;
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
    float dotNl = dot(normal, light) < 0.0f ? 0.0f : dot(normal, light);
    vec3 diffuse = color * dotNl * Ld;

    color = ambient +  diffuse;
}
