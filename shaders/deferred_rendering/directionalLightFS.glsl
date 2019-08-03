layout (location = 0) out vec4 FragColor;

layout (std140) uniform Lights {
    int numLights;
    Light lights[10];
};

layout (std140) uniform Camera {
    vec3 cameraPosition;
    vec3 cameraDirection;
};

vec3 phongComputeColor(Light light, vec3 diffuseColor, vec3 specularColor, float shininess, vec3 fragPosition, vec3 fragNormal, vec3 cameraPosition) {
    vec3 outcolor = vec3(0.0f);

    fragNormal = normalize(fragNormal);

    // ambient component
    outcolor += diffuseColor * light.ambientColor;

    /* The direction of a directional light is constant
     * Since rayToLight goes from the fragment to the light
     * light.direction must be inverted */
    vec3 rayToLight = -light.direction;
    
	// shadow mapping
	float inShadow = 0.0;
	if (light.castShadow) {
		vec4 lightSpacePos = light.toLightSpace * vec4(fragPosition, 1.0);
		inShadow = shadowMapIsInShadow(lightSpacePos, -rayToLight, fragNormal, distance(fragPosition, cameraPosition));
	}

    float diffuseIntensity = max(dot(fragNormal, rayToLight), 0.0f);
    outcolor += light.diffuseColor * diffuseColor * diffuseIntensity * (1.0 - inShadow);

    if (shininess != 0) { // Phong or Lambert material? if shininess == 0 it is Lambert
        // specular component
        vec3 rayToCamera = normalize(cameraPosition - fragPosition);
        vec3 halfWay = normalize(rayToCamera + rayToLight);

        float specularIntensity = pow(max(dot(halfWay, fragNormal), 0.0f), shininess);
        outcolor += light.specularColor * specularColor * specularIntensity * (1.0 - inShadow) * float(diffuseIntensity > 0);
    }

    return outcolor;
}

uniform sampler2D DiffuseData;
uniform sampler2D SpecularData;
uniform sampler2D PositionData;
uniform sampler2D NormalData;

uniform int lightIndex;

void main() {
    vec2 texCoord = gl_FragCoord.xy / textureSize(DiffuseData, 0);

    vec3 diffuseColor = texture(DiffuseData, texCoord).rgb;
    vec4 specularSample = texture(SpecularData, texCoord);
    vec3 specularColor = specularSample.rgb;
    float shininess = specularSample.a;
    vec3 normal = texture(NormalData, texCoord).xyz;
    vec3 position = texture(PositionData, texCoord).xyz;

    vec3 color = phongComputeColor(lights[lightIndex], diffuseColor, specularColor, shininess, position, normal, cameraPosition);
    FragColor = vec4(color, 1.0);
}
