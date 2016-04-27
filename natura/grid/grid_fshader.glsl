#version 330

in vec2 uv;
in vec3 light_dir;
in mat4 MV;

out vec3 color;

uniform sampler2D perlin_tex;
uniform sampler2D grass_tex;
uniform sampler2D rock_tex;
uniform sampler2D snow_tex;
uniform sampler2D sand_tex;
uniform vec3 La, Ld;
uniform vec3 ka, kd;

float getPercentage( float value,  float min,  float max ){
    value = clamp( value, min, max );
    return ( value - min ) / ( max - min );
}


void main() {
    float height = ((texture(perlin_tex, uv).r) + 1)/ 2.0f;
    vec3 grassColor = texture(grass_tex, uv* 5).rgb;
    vec3 rockColor = texture(rock_tex, uv* 5).rgb;
    vec3 snowColor = texture(snow_tex, uv* 5).rgb;
    vec3 sandColor = texture(sand_tex, uv* 5).rgb;



    if(height > 0.8){
        color = snowColor;
    }
    else if (height > 0.75){
        color = mix(rockColor, snowColor, getPercentage(height, 0.75f, 0.8f));
    }
    else if (height > 0.5){
        color = mix(grassColor, rockColor, getPercentage(height, 0.5f, 0.75f));
    }
    else if( height > 0.45f){
        color = mix(sandColor, grassColor, getPercentage(height, 0.45f, 0.5f));
    }
    else{
        color = sandColor;
    }

      float epsilon = 0.01f;
        float zDiffXaxis = texture(perlin_tex, vec2(uv.x + epsilon, uv.y)).r -
                        texture(perlin_tex, vec2(uv.x - epsilon, uv.y)).r;
        float zDiffYaxis = texture(perlin_tex, vec2(uv.x, uv.y + epsilon)).r -
                        texture(perlin_tex, vec2(uv.x, uv.y - epsilon)).r;

        vec3 normal = normalize(cross(vec3(2 *epsilon, zDiffXaxis, 0.0f), vec3(0.0, zDiffYaxis, 2* epsilon)));
        normal = (inverse(transpose(MV)) * vec4(-normal, 1.0f)).xyz;
        vec3 ambient = color * 0.6 * La;
        vec3 light = normalize(light_dir);
        float dotNl = dot(normal, light) < 0.0f ? 0.0f : dot(normal, light);
        vec3 diffuse = color * dotNl * Ld;
    color = ambient + diffuse;
}
