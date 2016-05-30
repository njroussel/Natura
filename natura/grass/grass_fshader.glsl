#version 330
#define min_fog_distance 30.0f
#define max_fog_distance 40.0f

in vec2 vTexCoord;
in float distance_camera;
out vec4 color;

uniform sampler2D gSampler;
uniform vec4 vColor;
uniform float fAlphaMultiplier;

void main()
{
    vec4 vTexColor = texture2D(gSampler, vec2(vTexCoord.x, -vTexCoord.y));
    float fNewAlpha = vTexColor.a;
    if(fNewAlpha < 0.1f) {
        discard;
    }

    if(fNewAlpha > 1.0f){
        fNewAlpha = 1.0f;
    }

    vec4 vMixedColor = vTexColor;

     float fog_factor = (max_fog_distance - distance_camera) / (max_fog_distance - min_fog_distance);
     fog_factor = clamp(fog_factor, 0.0f, 1.0f);

    color = vec4(vMixedColor.rgb, min(fNewAlpha, fog_factor));
}