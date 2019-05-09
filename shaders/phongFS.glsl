struct PhongMaterial {
    bool useDiffuseMap;
    sampler2D diffuse;
    vec3 diffuseColor;

    bool useSpecularMap;
    sampler2D specular;
    vec3 specularColor;

    float shininess;
};

in vec2 texCoord;
in vec3 position;
in vec3 normal;

out vec4 FragColor;

uniform vec3 cameraPosition;
uniform PhongMaterial material;
layout (std140) uniform Lights {
    int numLights;
    Light lights[10];
};

void main() {
    vec3 diffuseColor = material.diffuseColor;
    if (material.useDiffuseMap)
        diffuseColor *= vec3(texture2D(material.diffuse, texCoord));

    vec3 specularColor = material.specularColor;
    if (material.useSpecularMap)
        specularColor *= vec3(texture2D(material.specular, texCoord));

    vec3 color = vec3(0.0f);
    for (int i = 0; i < numLights; i++) {
        color += phongComputeColor(lights[i], diffuseColor, specularColor, material.shininess, position, normal, cameraPosition);
    }
    FragColor = vec4(color, 1.0f);
}