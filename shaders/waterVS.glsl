layout (location = 0) in vec3 vPos;
layout (location = 2) in vec2 vTexCoord;

uniform mat4 model;
layout (std140) uniform CommonMat {
    mat4 projection;
    mat4 view;
};

out vec4 clipSpaceCoord;
out vec2 texCoord;

void main() {
	texCoord = vTexCoord;
    gl_Position = projection * view * model * vec4(vPos, 1.0f);

	clipSpaceCoord = gl_Position;
}
