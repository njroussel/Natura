#version 330 core

in vec2 uv;

out vec3 color;

uniform vec2 displacement;
uniform float frequency ;
uniform float H;
uniform float offset;
uniform float lacunarity;
uniform int octaves;

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

const int p[256] = int[](173, 78, 203, 128, 97, 146, 63, 65, 159, 43, 212, 48, 34, 171, 183, 197,
    170, 69, 103, 216, 167, 208, 189, 93, 228, 49, 226, 59, 96, 156, 1, 72, 182, 188, 83, 166, 179, 143,
    47, 64, 193, 139, 28, 120, 231, 245, 52, 66, 21, 39, 90, 217, 151, 6, 45, 180, 210, 253, 87, 18,
    141, 114, 94, 177, 104, 8, 20, 181, 154, 205, 250, 227, 42, 106, 98, 117, 144, 15, 37, 229, 142, 35,
    25, 190, 248, 100, 62, 85, 194, 145, 110, 195, 137, 32, 56, 255, 74, 201, 80, 61, 24, 81, 38, 235,
    111, 162, 160, 23, 132, 134, 200, 73, 31, 118, 244, 163, 91, 135, 19, 169, 246, 175, 55, 149, 101,
    207, 233, 238, 199, 16, 153, 147, 254, 75, 220, 79, 185, 165, 99, 206, 124, 4, 60, 186, 22, 17, 148,
    202, 236, 196, 158, 51, 10, 127, 76, 176, 92, 232, 12, 213, 107, 27, 84, 164, 123, 178, 109, 204,
    133, 108, 41, 222, 2, 105, 57, 77, 209, 67, 218, 122, 225, 211, 150, 14, 242, 168, 121, 136, 224,
    9, 102, 116, 157, 112, 33, 125, 7, 130, 161, 50, 30, 29, 140, 70, 86, 0, 219, 172, 129, 58, 252, 184,
    221, 46, 240, 192, 113, 187, 11, 71, 251, 89, 249, 131, 234, 198, 138, 54, 44, 95, 13, 247, 5, 243,
    152, 26, 68, 237, 82, 53, 115, 40, 215, 191, 223, 3, 241, 239, 36, 126, 214, 119, 230, 174, 88, 155);

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
    return sum / range;
}

void main(){
    vec2 point = vec2((uv[0]+displacement.x*2), (uv[1]+displacement.y*2));
    color = vec3(multifractal(point), 0.0f, 0.0f);
}