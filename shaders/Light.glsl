// Light types definition
const uint LIGHT_TYPE_POINT = 0;
const uint LIGHT_TYPE_DIRECTIONAL = 1;
const uint LIGHT_TYPE_SPOT = 2;

struct Light {
    uint type;             // 16 + 128 * lightIndex
    vec3 position;         // 32
    vec3 direction;        // 48

    vec3 ambientColor;     // 64
    vec3 diffuseColor;     // 80
    vec3 specularColor;    // 96
    vec3 attenuations;     // 112

    vec2 spotAngles;       // 128
                           // total 144
};
