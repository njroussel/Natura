#version 330 core

uniform vec2 displacement;
uniform float frequency ;
uniform float H;
uniform float offset;
uniform float lacunarity;
uniform int octaves;

in vec2 uv; // Texture coordinates
out vec3 color;

vec4 permute ( vec4 x) {
    return mod ((34.0 * x + 1.0) * x , 289.0) ;
}

vec2 cellular2x2 ( vec2 P) {
    const float K = 1.0f / 7.0f;
    const float K2 = 0.5f / 7.0f;
    const float jitter = 0.8f; // jitter 1.0 makes F1 wrong more often
    vec2 Pi = mod ( floor (P ) , 289.0f) ;
    vec2 Pf = fract ( P);
    vec4 Pfx = Pf .x + vec4 ( -0.5 , -1.5 , -0.5 , -1.5) ;
    vec4 Pfy = Pf .y + vec4 ( -0.5 , -0.5 , -1.5 , -1.5) ;
    vec4 p = permute ( Pi .x + vec4 (0.0 , 1.0 , 0.0 , 1.0) );
    p = permute (p + Pi .y + vec4 (0.0 , 0.0 , 1.0 , 1.0) );
    vec4 ox = mod (p , 7.0) *K+ K2 ;
    vec4 oy = mod ( floor (p *K) ,7.0) * K+ K2 ;
    vec4 dx = Pfx + jitter * ox ;
    vec4 dy = Pfy + jitter * oy ;
    vec4 d = dx * dx + dy * dy ; // distances squared
    // Cheat and pick only F1 for the return value
    d. xy = min (d.xy , d. zw ) ;
    d.x = min (d.x , d. y);
    return d. xx ; // F1 duplicated , F2 not computed
}


float multifractal(vec2 point) {
    float sum = cellular2x2(point * frequency).x*2;
    float amplitude = 1.f;
    float range = 2.f;
    float tmpFreq = frequency;
    for (int i = 1; i < octaves; i++) {
        tmpFreq *= lacunarity;
    	amplitude *= H;
    	range += amplitude;
    	sum += ((cellular2x2(point * tmpFreq).x) + offset) * amplitude;
    }
    return (sum / range);
}


void main ( void ) {
    color = vec3 (multifractal(uv), 0.0f ,0.0f);
}