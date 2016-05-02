#version 330 core
uniform sampler2D tex;
uniform samplerCube tex_cube;
in vec3 uv;
out vec3 color;

void main(){
    color = texture(tex_cube, uv).rgb;
}
