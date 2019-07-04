in vec2 texCoord;

layout (std140) uniform Lights {
    int numLights;
    Light lights[10];
};

layout (std140) uniform Camera {
    vec3 cameraPosition;
    vec3 cameraDirection;
};

uniform sampler2D DiffuseData;
uniform sampler2D SpecularData;
uniform sampler2D PositionData;
uniform sampler2D NormalData;

layout (std140) uniform CommonMat {
    mat4 projection;
    mat4 view;
	mat4 shadowLightSpace;
};

out vec4 FragColor;

void main() {
    vec3 position = texture(PositionData, texCoord).rgb;
    vec3 normal = texture(NormalData, texCoord).rgb;
    vec4 diffuseSample = texture(DiffuseData, texCoord);
    vec3 diffuseColor = diffuseSample.rgb;
    vec4 specularSample = texture(SpecularData, texCoord);
    vec3 specularColor = specularSample.rgb;
    float shininess = specularSample.a;

    vec4 lightSpacePosition = shadowLightSpace * vec4(position, 1.0);

    vec3 color = vec3(0.0);
    for (int i = 0; i < numLights; i++) 
        color += phongComputeColor(lights[i], diffuseColor, specularColor, shininess, position, normal, cameraPosition, lightSpacePosition, i == 0);

	// apply fog
	color = fogger(color, distance(cameraPosition, position));

    FragColor = vec4(color, 1.0);
}
