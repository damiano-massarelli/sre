layout (location = 0) out vec4 Diffuse;
layout (location = 1) out vec4 PBRData;
layout (location = 2) out vec3 Position;
layout (location = 3) out vec3 Normal;

struct PBRMaterial {
    sampler2D albedo;
    sampler2D normal;
    sampler2D roughness;
    sampler2D metalness;
    sampler2D ao;
};

in vec2 texCoord;
in vec3 position;

// bump mapping specific ins
in mat3 tangentToWorldSpace;

uniform PBRMaterial material;

void main() {
    // get normal from bump map
    vec3 normal = (texture(material.normal, texCoord).rgb) * 2.0 - 1.0;
    normal = normalize(tangentToWorldSpace * normal);

	Position = position;
	Normal = normal;

    Diffuse = texture(material.albedo, texCoord);

    PBRData.x = texture(material.roughness, texCoord).r;
	PBRData.y = texture(material.metalness, texCoord).r;
    PBRData.z = texture(material.ao, texCoord).r;
}
