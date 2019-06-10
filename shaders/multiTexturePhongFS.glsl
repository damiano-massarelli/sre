in vec2 texCoord;
in vec3 position;
in vec3 normal;
in vec4 lightSpacePosition;

out vec4 FragColor;

uniform sampler2D baseTexture;
uniform sampler2D redTexture;
uniform sampler2D greenTexture;
uniform sampler2D blueTexture;
uniform sampler2D blendTexture;

uniform sampler2D shadowMap;

uniform float horizontalTiles;
uniform float verticalTiles;

layout (std140) uniform Lights {
    int numLights;
    Light lights[10];
};

layout (std140) uniform Camera {
    vec3 cameraPosition;
    vec3 cameraDirection;
};

float shadowCalculation(vec4 lightSpacePos) {
	vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
	projCoords = projCoords * 0.5 + 0.5;

	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;

	return (currentDepth - 0.005 > closestDepth ? 1.0f : 0.0f);
}

void main() {
    vec4 channels = texture2D(blendTexture, texCoord / vec2(horizontalTiles, verticalTiles));
    float baseFactor = 1.0f - (channels.r + channels.g + channels.b);

    vec3 base = vec3(texture2D(baseTexture, texCoord));
    vec3 red = vec3(texture2D(redTexture, texCoord));
    vec3 green = vec3(texture2D(greenTexture, texCoord));
    vec3 blue = vec3(texture2D(blueTexture, texCoord));

    vec3 diffuseColor = base * baseFactor + red * channels.r + green * channels.g + blue * channels.b; 

    // gamma correction
	diffuseColor = pow(diffuseColor, vec3(2.2));
    vec3 color = vec3(0.0f);
    for (int i = 0; i < numLights; i++) {
        color += phongComputeColor(lights[i], diffuseColor, vec3(0.0f), 0.0f, position, normal, cameraPosition);
    }

	float shadow = shadowCalculation(lightSpacePosition);
	color *= (1 - shadow);

    float fogFactor = exp(-pow(distance(position, cameraPosition) * 0.007f, 1.5f));
    color = mix(color, vec3(0.5f), 1 - fogFactor);
    FragColor = vec4(color, 1.0f);
}
