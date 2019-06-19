layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNorm;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in ivec4 vBones;
layout (location = 4) in vec4 vWeights;

uniform mat4 model;
layout (std140) uniform CommonMat {
    mat4 projection;
    mat4 view;
	mat4 shadowLightSpace;
};

uniform mat4 bones[40];

out vec2 texCoord;
out vec3 position;
out vec3 normal;
out vec4 lightSpacePosition;

void main() {
    texCoord = vTexCoord;
	mat4 boneTransform = bones[vBones[0]] * vWeights[0] +
						 bones[vBones[1]] * vWeights[1]	+
						 bones[vBones[2]] * vWeights[2] +
						 bones[vBones[3]] * vWeights[3];


    position = mat3(1.3) * (model * boneTransform * vec4(vPos, 1.0f)).xyz;
    normal = normalize(inverse(transpose(mat3(model))) * vNorm);

	lightSpacePosition = shadowLightSpace * vec4(position, 1.0f);

    gl_Position = projection * view * vec4(position, 1.0f);
}
