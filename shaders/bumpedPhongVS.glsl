layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNorm;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in vec3 vTangent;

uniform mat4 model;
layout (std140) uniform CommonMat {
    mat4 projection;
    mat4 view;
	mat4 shadowLightSpace;
};

out vec2 texCoord;
out vec3 position;
out vec4 lightSpacePosition;

//debuggg
out vec3 tgn;

// bump mapping specific outs
out mat3 tangentToWorldSpace;
 
void main() {
	// debugg 
	tgn = vTangent;

    texCoord = vTexCoord;

    position = (model * vec4(vPos, 1.0f)).xyz;

    mat3 normalMatrix = inverse(transpose(mat3(model)));
    vec3 normal = normalize(normalMatrix * vNorm);

    vec3 tangent = normalize(normalMatrix * vTangent);
    tangent = normalize(tangent - (dot(tangent, normal) * normal)); // ortogonalize it

    vec3 bitangent = cross(normal, tangent);

    tangentToWorldSpace = mat3(tangent, bitangent, normal);

	lightSpacePosition = shadowLightSpace * vec4(position, 1.0f);

    gl_Position = projection * view * vec4(position, 1.0f);
}
