layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNorm;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in vec3 vTangent;

uniform mat4 model;
uniform mat3 normalModel;

layout (std140) uniform CommonMat {
    mat4 projection;
    mat4 view;
	mat4 projectionView;
	mat4 shadowLightSpace;
	vec4 clipPlane;
};

layout (std140) uniform Camera {
    vec3 cameraPosition;
    vec3 cameraDirection;
};

out vec2 texCoord;
out vec3 position;

// bump mapping specific outs
out mat3 tangentToWorldSpace;
out vec3 tangentSpaceRayToCamera;

void main() {
	gl_ClipDistance[0] = dot(vec4(position, 1.0), clipPlane);

    texCoord = vTexCoord;

    position = (model * vec4(vPos, 1.0f)).xyz;

    vec3 normal = normalize(normalModel * vNorm);

    vec3 tangent = normalize(normalModel * vTangent);
    tangent = normalize(tangent - (dot(tangent, normal) * normal)); // ortogonalize it

    vec3 bitangent = cross(normal, tangent);

    tangentToWorldSpace = mat3(tangent, bitangent, normal);
    tangentSpaceRayToCamera = transpose(tangentToWorldSpace) * normalize(cameraPosition - position);

    gl_Position = projectionView * vec4(position, 1.0f);
}
