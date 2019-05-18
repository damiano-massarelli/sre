layout (location = 0) in vec3 vPos;

uniform mat4 model;
layout (std140) uniform CommonMat {
    mat4 projection;
    mat4 view;
};

out vec3 position;

void main() {
    position = vPos;

    mat4 noMoveView = view;
    noMoveView[3] = vec4(0, 0, 0, 1);
    vec4 position = projection * noMoveView * model * vec4(vPos, 1.0f);

    // make sure its depth is 1 (wont be in front of anything)
    gl_Position = position.xyww;
}
