struct PhongMaterial {
    bool useDiffuseMap;
    sampler2D diffuse;
    vec3 diffuseColor;

    bool useSpecularMap;
    sampler2D specular;
    vec3 specularColor;

    float shininess;

    float opacity;
};

in vec2 texCoord;
in vec3 position;
in vec3 normal;
in vec4 lightSpacePosition;

out vec4 FragColor;

uniform PhongMaterial material;

layout (std140) uniform Lights {
    int numLights;
    Light lights[10];
};

layout (std140) uniform Camera {
    vec3 cameraPosition;
    vec3 cameraDirection;
};

void main() {
	vec3 shadowSampleCoord = lightSpacePosition.xyz / lightSpacePosition.w;
	shadowSampleCoord = (shadowSampleCoord + vec3(1.0)) / 2.0;

	// TODO check the value of w to see if
	// perspective projection is being used. If that's the case linearize depth.
	float depthInShadowMap = texture(shadowMap, shadowSampleCoord.xy).r;

    if (material.opacity == 0.0f) discard;

    vec3 diffuseColor = material.diffuseColor;
    if (material.useDiffuseMap)
        diffuseColor *= vec3(texture2D(material.diffuse, texCoord));

    vec3 specularColor = material.specularColor;
    if (material.useSpecularMap)
        specularColor *= vec3(texture2D(material.specular, texCoord));

	diffuseColor = pow(diffuseColor, vec3(2.2));
	specularColor = pow(specularColor, vec3(2.2));
    vec3 color = vec3(0.0f);
    for (int i = 0; i < numLights; i++) {
        color += phongComputeColor(lights[i], diffuseColor, specularColor, material.shininess, position, normal, cameraPosition, lightSpacePosition, i == 0);
    }

    //float fogFactor = exp(-pow(distance(position, cameraPosition) * 0.007f, 1.5f));
    //color = mix(color, vec3(0.2f), 1 - fogFactor);

    FragColor = vec4(color, material.opacity);
}
