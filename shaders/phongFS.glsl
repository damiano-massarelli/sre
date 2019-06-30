layout (location = 0) out vec4 Diffuse;
layout (location = 1) out vec4 Specular;
layout (location = 2) out vec3 Position;
layout (location = 3) out vec3 Normal;

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

uniform PhongMaterial material;

void main() {
	Position = position;
	Normal = normal;

	vec4 sampledDiffuseColor = texture(material.diffuse, texCoord);
	if (material.opacity == 0.0f || sampledDiffuseColor.a < 0.5) discard;

    Diffuse = vec4(material.diffuseColor, 1.0);
    if (material.useDiffuseMap)
        Diffuse.rgb *= sampledDiffuseColor.rgb;

    Specular = vec4(material.specularColor, material.shininess);
    if (material.useSpecularMap)
        Specular.rgb *= vec3(texture(material.specular, texCoord));
}
