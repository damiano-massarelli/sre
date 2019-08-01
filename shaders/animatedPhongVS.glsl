layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNorm;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in ivec4 vBones;
layout (location = 4) in vec4 vWeights;

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
out vec3 normal;

void main() {
	gl_ClipDistance[0] = dot(vec4(position, 1.0), clipPlane);

    texCoord = vTexCoord;
	mat4 boneTransform = bones[vBones[0]] * vWeights[0] +
						 bones[vBones[1]] * vWeights[1]	+
						 bones[vBones[2]] * vWeights[2] +
						 bones[vBones[3]] * vWeights[3];


    position = (model * boneTransform * vec4(vPos, 1.0f)).xyz;
    normal = normalize(normalModel * vNorm);

    gl_Position = projectionView * vec4(position, 1.0f);
}
