#version 330

in vec3 vpoint;

uniform float amplitude;

void main()
{
   gl_Position = vec4(vec3(vpoint.x, vpoint.y * amplitude, vpoint.z), 1.0);
}