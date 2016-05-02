#version 330 core
uniform mat4 MVP;
uniform mat4 M;
in vec3 vpoint;
out vec3 uv;

void main(){
    vec4 pos = MVP * vec4(vpoint, 1.0f);
    gl_Position =  pos;
    uv = vec3(M * vec4(vpoint, 1.0f));
}
