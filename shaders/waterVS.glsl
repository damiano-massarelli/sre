layout (location = 0) in vec3 vPos;
layout (location = 2) in vec2 vTexCoord;

uniform mat4 model;
layout (std140) uniform CommonMat {
    mat4 projection;
    mat4 view;
	mat4 projectionView;
};

out vec4 clipSpaceCoord;
out vec2 texCoord;
out vec3 position;

void main() {
	texCoord = vTexCoord;
	vec4 pos = model * vec4(vPos, 1.0f);
	position = pos.xyz;
    gl_Position = projectionView * pos;

	clipSpaceCoord = gl_Position;
}
