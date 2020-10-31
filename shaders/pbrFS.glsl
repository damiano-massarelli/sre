layout (location = 0) out vec3 Diffuse;
layout (location = 1) out vec3 PBRData;
layout (location = 2) out vec3 Position;
layout (location = 3) out vec3 Normal;

struct PBRMaterial {
    sampler2D albedoMap;
    sampler2D normalMap;
    sampler2D roughnessMap;
    sampler2D metalnessMap;
    sampler2D aoMap;

    vec3 albedo;
	float metalness;
	float roughness;
	float ao;

    bool useAlbedoMap;
    bool useNormalMap;
    bool useMetalnessMap;
    bool useRoughnessMap;
    bool useAOMap;
};

in vec2 texCoord;
in vec3 position;

// bump mapping specific ins
in mat3 tangentToWorldSpace;

uniform PBRMaterial material;

void main() {
    Position = position;

    // The normal is in the third column
    vec3 normal = tangentToWorldSpace[2];
    if (material.useNormalMap) {
        normal = texture(material.normalMap, texCoord).rgb * 2.0 - 1.0;
        normal = normalize(tangentToWorldSpace * normal);
    }    
	Normal = normal;

    Diffuse = material.albedo;
    if (material.useAlbedoMap) {
        Diffuse *= texture(material.albedoMap, texCoord).rgb;
    }
    Diffuse.rgb = pow(Diffuse.rgb, vec3(2.2));
    
    PBRData.x = material.roughness;
    if (material.useRoughnessMap) {
        PBRData.x *= texture(material.roughnessMap, texCoord).r;
    }
    
    PBRData.y = material.metalness;
    if (material.useMetalnessMap) {
        PBRData.y *= texture(material.metalnessMap, texCoord).r;
    }

	PBRData.z = material.ao;
    if (material.useAOMap) {
        PBRData.z *= texture(material.aoMap, texCoord).r;
    }
}
