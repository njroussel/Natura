#version 330

in vec2 vTexCoord;

in vec3 vWorldPos;
in vec4 vEyeSpacePos;

out vec4 outputColor;

uniform sampler2D gSampler;
uniform vec4 vColor;
uniform vec3 vEyePosition;
uniform float fAlphaTest;
uniform float fAlphaMultiplier;

void main()
{
   vec4 vTexColor = texture2D(gSampler, vTexCoord);
   float fNewAlpha = vTexColor.a*fAlphaMultiplier;
   if(fNewAlpha < fAlphaTest)
      discard;

   if(fNewAlpha > 1.0f)
      fNewAlpha = 1.0f;

   vec4 vMixedColor = vTexColor*vColor;

   outputColor = vec4(vMixedColor.zyx, fNewAlpha);
}