#version 330 

in vec3 vpoint;

uniform mat4 depth_vp;
uniform mat4 model;

void main() {
    gl_Position = depth_vp * model * vec4(vpoint, 1.0);
}
