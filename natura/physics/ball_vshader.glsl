#version 330

in vec3 vpoint;
in vec3 vnormal;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

out vec3 view_dir;
out vec3 light_dir;
out vec3 normal_mv;
out float distance_camera;


void main() {
    mat4 MV = view * model;
    vec4 vpoint_mv = MV * vec4(vpoint, 1.0);
    gl_Position = projection * vpoint_mv;
    distance_camera = length(vpoint_mv);


    ///>>>>>>>>>> TODO >>>>>>>>>>>
    /// TODO 1.1: Phong shading.
    /// 1) compute normal_mv using the model_view matrix.
    normal_mv = (inverse(transpose(MV)) * vec4(vnormal, 1.0f)).xyz;
    normal_mv = normalize(normal_mv);

    /// 2) compute the light direction light_dir.
    light_dir = -vpoint_mv.xyz;
    light_dir = normalize(light_dir);

    /// 3) compute the view direction view_dir.
    view_dir = -vpoint_mv.xyz;
    view_dir  = normalize(view_dir);
    ///<<<<<<<<<< TODO <<<<<<<<<<<
}
