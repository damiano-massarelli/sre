layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNorm;
layout (location = 2) in vec2 vTexCoord;

uniform mat4 model;
layout (std140) uniform CommonMat {
    mat4 projection;
    mat4 view;
};

out vec2 texCoord;
out vec3 position;
out vec3 normal;

void main() {
    texCoord = vTexCoord;

    position = (model * vec4(vPos, 1.0f)).xyz;
    normal = normalize(inverse(transpose(mat3(model))) * vNorm);

    gl_Position = projection * view * vec4(position, 1.0f);
}
