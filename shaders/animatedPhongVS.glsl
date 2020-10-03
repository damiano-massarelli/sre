layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNorm;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in vec3 vTangent;
layout (location = 4) in ivec4 vBones;
layout (location = 5) in vec4 vWeights;

uniform mat4 model;
uniform mat3 normalModel;

layout (std140) uniform CommonMat {
    mat4 projection;
    mat4 view;
	mat4 projectionView;
	vec4 clipPlane;
};

uniform mat4 bones[100];

out vec2 texCoord;
out vec3 position;

// bump mapping specific outs
out mat3 tangentToWorldSpace;

void main() {
    texCoord = vTexCoord;
	mat4 boneTransform = bones[vBones[0]] * vWeights[0] +
						 bones[vBones[1]] * vWeights[1]	+
						 bones[vBones[2]] * vWeights[2] +
						 bones[vBones[3]] * vWeights[3];


    position = (model * boneTransform * vec4(vPos, 1.0f)).xyz;

	mat3 normalBoneTransform = normalModel * mat3(boneTransform);
	vec3 normal = normalize(normalBoneTransform * vNorm);

	vec3 tangent = normalize(normalBoneTransform * vTangent);
	tangent = normalize(tangent - (dot(tangent, normal) * normal)); // ortogonalize it

    vec3 bitangent = cross(normal, tangent);

    tangentToWorldSpace = mat3(tangent, bitangent, normal);

	gl_ClipDistance[0] = dot(vec4(position, 1.0), clipPlane);
    normal = normalize(normalModel * vNorm);

    gl_Position = projectionView * vec4(position, 1.0f);
}
