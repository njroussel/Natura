#version 330
#define noise_size 4.0f

in vec3 vpoint;


uniform float amplitude;
uniform sampler2D perlin_tex;

void main()
{
   vec2 pos_2d = vec2(vpoint.x / noise_size, vpoint.z / noise_size) ;
   float height = amplitude * (texture(perlin_tex, pos_2d).r - 0.5);
   vec3 pos_3d = vec3(vpoint.x, height, vpoint.z);
   gl_Position = vec4(pos_3d, 1.0);

}