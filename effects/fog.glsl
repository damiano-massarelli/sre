layout (std140) uniform Camera {
    vec3 cameraPosition;
    float cameraNear;
    vec3 cameraDirection;
    float cameraFar;
};

in vec2 texCoord;

uniform vec3 uFogColor;
uniform vec2 uFogParams;
uniform sampler2D uInputTexture;
uniform sampler2D uDepthTexture;

out vec4 fragColor;

void main() {
    vec4 color = texture(uInputTexture, texCoord);
    float z = texture(uDepthTexture, texCoord).r;

    // Linear depth
    z = 2.0 * z - 1.0;
    float dist = 2.0 * cameraNear * cameraFar / (cameraFar + cameraNear - z * (cameraFar - cameraNear));

    // 0 means not in fog, 1 means in fog
    float fogFactor = 1 - exp(-pow(dist * uFogParams.x, uFogParams.y));
    
    color.xyz = mix(color.xyz, uFogColor, fogFactor);

    fragColor = color;
}
