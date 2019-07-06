layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNorm;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in vec3 vTangent;

uniform mat4 model;
layout (std140) uniform CommonMat {
    mat4 projection;
    mat4 view;
	mat4 shadowLightSpace;
	vec4 clipPlane;
};

out vec2 texCoord;
out vec3 position;

// bump mapping specific outs
out mat3 tangentToWorldSpace;
 
void main() {
    texCoord = vTexCoord;

    position = (model * vec4(vPos, 1.0f)).xyz;

    mat3 normalMatrix = inverse(transpose(mat3(model)));
    vec3 normal = normalize(normalMatrix * vNorm);

    vec3 tangent = normalize(normalMatrix * vTangent);
    tangent = normalize(tangent - (dot(tangent, normal) * normal)); // ortogonalize it

    vec3 bitangent = cross(normal, tangent);

    tangentToWorldSpace = mat3(tangent, bitangent, normal);

	gl_ClipDistance[0] = dot(vec4(position, 1.0), clipPlane);

    gl_Position = projection * view * vec4(position, 1.0f);
}
