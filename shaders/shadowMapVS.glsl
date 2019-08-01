/** This shader is used for when objects are
  * rendered for shadow mapping */
layout (location = 0) in vec3 vPos;

uniform mat4 model;
layout (std140) uniform CommonMat {
    mat4 projection;
    mat4 view;
	mat4 projectionView;
};

void main() {
    gl_Position = projectionView * model * vec4(vPos, 1.0f);
}
