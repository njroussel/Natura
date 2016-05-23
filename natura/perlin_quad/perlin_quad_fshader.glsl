#version 330 core

in vec2 uv;

out vec3 color;

uniform vec2 displacement;
uniform float frequency ;
uniform float H;
uniform float offset;
uniform float lacunarity;
uniform int octaves;

uniform int p[256];

float fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float lerp(float t, float a, float b) {
    return a + t * (b - a);
}

float grad(int hash, float x, float y, float z) {
    int h = hash & 15;                      // CONVERT LO 4 BITS OF HASH CODE
    float u = h < 8 ? x : y,                 // INTO 12 GRADIENT DIRECTIONS.
           v = h<4 ? y : h==12||h==14 ? x : z;
    return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

float perlinNoise(vec2 point, float freq) {
    float x = point.x * freq;
    float y = point.y * freq;
    float z = 0;                                  // OF POINT IN CUBE.
    int X =  int(floor(x)) & 255;                 // FIND UNIT CUBE THAT
    int Y =  int(floor(y)) & 255;                 // CONTAINS POINT.
    int Z =  int(floor(z)) & 255;
    x -=  floor(x);                               // FIND RELATIVE X,Y,Z
    y -=  floor(y);                               // OF POINT IN CUBE.
    z -=  floor(z);                               // OF POINT IN CUBE.
    float u = fade(x),                            // COMPUTE FADE CURVES
            v = fade(y),
            w = fade(z);                          // FOR EACH OF X,Y,Z.

    int A = p[(X  ) % 256]+Y, AA = p[A % 256]+Z, AB = p[(A+1) % 256]+Z, // HASH COORDINATES OF
          B = p[(X+1) % 256]+Y, BA = p[B % 256]+Z, BB = p[(B+1) % 256]+Z; // THE 8 CUBE CORNERS,

    float value = lerp(w, lerp(v, lerp(u, grad(p[(AA  ) % 256], x  , y  , z   ),   // AND ADD
                                     grad(p[(BA  ) % 256], x-1, y  , z   )),        // BLENDED
                             lerp(u, grad(p[(AB  ) % 256], x  , y-1, z   ),         // RESULTS
                                     grad(p[(BB  ) % 256], x-1, y-1, z   ))),       // FROM  8
                     lerp(v, lerp(u, grad(p[(AA+1) % 256], x  , y  , z-1 ),         // CORNERS
                                     grad(p[(BA+1) % 256], x-1, y  , z-1 )),        // OF CUBE
                             lerp(u, grad(p[(AB+1) % 256], x  , y-1, z-1 ),
                                     grad(p[(BB+1) % 256], x-1, y-1, z-1 ))));
    return value;
}


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
    float sum = perlinNoise(point, frequency)*2;
    float amplitude = 1.f;
    float range = 2.f;
    float tmpFreq = frequency;
    for (int i = 1; i < octaves; i++) {
        tmpFreq *= lacunarity;
    	amplitude *= H;
    	range += amplitude;
    	sum += ((1.0 - abs(perlinNoise(point, tmpFreq))) * 2 + offset) * amplitude;
    }
    return (sum / range);
}

void main(){
    vec2 point = vec2((uv[0]+displacement.x*2), (uv[1]+displacement.y*2));
    color = vec3(multifractal(point), 0.0f, 0.0f);
}