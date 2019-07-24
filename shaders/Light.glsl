// Light types definition
const uint LIGHT_TYPE_POINT = 0;
const uint LIGHT_TYPE_DIRECTIONAL = 1;
const uint LIGHT_TYPE_SPOT = 2;

struct Light {
						   // where data ends			where data starts
    uint type;             // 16 + 196 * lightIndex		0
    vec3 position;         // 32						16
    vec3 direction;        // 48						32

    vec3 ambientColor;     // 64						48
    vec3 diffuseColor;     // 80						64
    vec3 specularColor;    // 96						80
    vec3 attenuations;     // 112						96

    vec2 spotAngles;       // 128						112
	mat4 toLightSpace;     // 192						128
	bool castShadow;	   // 208						192	
};
