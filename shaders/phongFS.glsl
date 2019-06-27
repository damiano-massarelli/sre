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
	vec4 sampledDiffuseColor = texture(material.diffuse, texCoord);
	if (material.opacity == 0.0f || sampledDiffuseColor.a < 0.01) discard;

    vec3 diffuseColor = material.diffuseColor;
    if (material.useDiffuseMap)
        diffuseColor *= sampledDiffuseColor.rgb;

    vec3 specularColor = material.specularColor;
    if (material.useSpecularMap)
        specularColor *= vec3(texture(material.specular, texCoord));

	//diffuseColor = pow(diffuseColor, vec3(2.2));
	//specularColor = pow(specularColor, vec3(2.2));
    vec3 color = vec3(0.0f);
    for (int i = 0; i < numLights; i++) {
        color += phongComputeColor(lights[i], diffuseColor, specularColor, material.shininess, position, normal, cameraPosition, lightSpacePosition, i == 0);
    }

	// apply fog
	color = fogger(color, distance(cameraPosition, position));

    FragColor = vec4(color, material.opacity);
}
