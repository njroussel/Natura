#version 330

in vec2 vTexCoord;

out vec4 color;

uniform sampler2D gSampler;
uniform vec4 vColor;
uniform float fAlphaMultiplier;

void main()
{
   vec4 vTexColor = texture2D(gSampler, vTexCoord);
   float fNewAlpha = vTexColor.a*fAlphaMultiplier;


   if(fNewAlpha > 1.0f)
      fNewAlpha = 1.0f;

   vec4 vMixedColor = vTexColor*vColor;

   //outputColor = vec4(vMixedColor.zyx, fNewAlpha);
  color = vec4(1.0f);
}