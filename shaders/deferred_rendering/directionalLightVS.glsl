/** Shader used for directional light pass in deferred rendering */
layout (location = 0) in vec2 vPos;
layout (location = 0) in vec2 vTexCoord;

out vec2 texCoord;

void main() {
	texCoord = vTexCoord;
    gl_Position = vec4(vPos, 0.0, 1.0);
}
