#version 330

in vec2 vTexCoord;

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

    color = vec4(vMixedColor.rgb, fNewAlpha);
}