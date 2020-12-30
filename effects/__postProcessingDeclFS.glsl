in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D screenTexture;
uniform sampler2D depthTexture;

layout (std140) uniform Camera {
    vec3 cameraPosition;
    float cameraNear;
    vec3 cameraDirection;
    float cameraFar;
};
