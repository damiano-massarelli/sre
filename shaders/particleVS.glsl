layout (location = 0) in vec3 vPos;

uniform mat4 model;
layout (std140) uniform CommonMat {
    mat4 projection;
    mat4 view;
	mat4 projectionView;
};

out vec2 texCoord;

void main() {
    texCoord = vPos.xy + 0.5;
    texCoord.y = 1.0 - texCoord.y;
    gl_Position = projectionView * model * vec4(vPos, 1.0f);
}
