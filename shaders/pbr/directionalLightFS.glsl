layout (location = 0) out vec4 FragColor;

layout (std140) uniform Lights {
    int numLights;
    Light lights[10];
};

layout (std140) uniform Camera {
    vec3 cameraPosition;
    vec3 cameraDirection;
};

uniform sampler2D DiffuseData;
uniform sampler2D PBRData;
uniform sampler2D PositionData;
uniform sampler2D NormalData;

uniform int lightIndex;

void main() {
    vec2 texCoord = gl_FragCoord.xy / textureSize(DiffuseData, 0);

    vec3 albedo = texture(DiffuseData, texCoord).rgb;
    vec4 data = texture(PBRData, texCoord);
    vec3 normal = texture(NormalData, texCoord).xyz;
    vec3 position = texture(PositionData, texCoord).xyz;

    Light light = lights[lightIndex];

    vec3 L = -light.direction;

    float inShadow = 0.0;
    if (light.castShadow)
        inShadow = shadowMapIsInShadow(light.toLightSpace * vec4(position, 1.0), light.direction, normal, distance(cameraPosition, position));

    vec3 color = pbrComputeColor(light, L, 1.0, inShadow, albedo, data.x, data.y, data.z, position, normal, cameraPosition);

    FragColor = vec4(color, 1.0);
}
