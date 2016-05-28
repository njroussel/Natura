#version 330

in vec3 normal_mv;
in vec3 view_dir;
in vec3 light_dir;


out vec3 color;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;


void main() {
    //>>>>>>>>>> TODO >>>>>>>>>>>
    // TODO 1.2: Phong shading.
    // 1) compute ambient term.
    /*vec3 ambient = ka * La;

    // 2) compute diffuse term.
    float dotNl = dot(normal_mv, light_dir) < 0.0f ? 0.0f : dot(normal_mv, light_dir);
    vec3 diffuse = kd * dotNl * Ld;

    // 3) compute specular term.
    vec3 r = 2 * normal_mv * dotNl - light_dir;
    r = normalize(r);
    float dotRv = dot(r, view_dir) < 0.0f ? 0.0f : dot(r, view_dir);

    vec3 specular = ks * pow(dotRv, alpha) * Ls;*/
    color = vec3(0.0f);// ambient + diffuse + specular;
    //<<<<<<<<<< TODO <<<<<<<<<<<
}
