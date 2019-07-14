layout (location = 0) in vec3 vPos;
layout (location = 1) in mat4 iTransform;
layout (location = 5) in vec4 iOffsets;
layout (location = 6) in float iBlendFactor;

layout (std140) uniform CommonMat {
    mat4 projection;
    mat4 view;
	mat4 projectionView;
};

out vec2 texCoord;
out float blend;
out vec4 offsets;

void main() {
    texCoord = vPos.xy + 0.5;
    texCoord.y = 1.0 - texCoord.y;

    blend = iBlendFactor;
    offsets = iOffsets;

    gl_Position = projectionView * iTransform * vec4(vPos + gl_InstanceID, 1.0f);
}
