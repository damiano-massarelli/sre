/** This shader is used for light props or objects
  * that should be rendered using a single color not affected by lights */
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNorm;
layout (location = 2) in vec2 vTexCoord;

uniform mat4 model;
layout (std140) uniform CommonMat {
    mat4 projection;
    mat4 view;
	mat4 projectionView;
};

out vec3 normal;

void main() {
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    normal = normalize(vec3(projection * vec4(normalMatrix * vNorm, 0.0)));

    gl_Position = projectionView * model * vec4(vPos, 1.0f);
}
