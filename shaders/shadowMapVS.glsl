/** This shader is used for when objects are
  * rendered for shadow mapping */
layout (location = 0) in vec3 vPos;

uniform mat4 model;
layout (std140) uniform CommonMat {
    mat4 projection;
    mat4 view;
};

void main() {
    gl_Position = projection * view * model * vec4(vPos, 1.0f);
}
