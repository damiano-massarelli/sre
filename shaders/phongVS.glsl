layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNorm;
layout (location = 2) in vec2 vTexCoord;

uniform mat4 model;
uniform mat3 normalModel;

layout (std140) uniform CommonMat {
    mat4 projection;
    mat4 view;
	mat4 projectionView;
	mat4 shadowLightSpace;
	vec4 clipPlane;
};

out vec2 texCoord;
out vec3 position;
out vec3 normal;

void main() {
    texCoord = vTexCoord;

    position = (model * vec4(vPos, 1.0f)).xyz;
    normal = normalize(normalModel * vNorm);


	gl_ClipDistance[0] = dot(vec4(position, 1.0), clipPlane);

    gl_Position = projectionView * vec4(position, 1.0f);
}
