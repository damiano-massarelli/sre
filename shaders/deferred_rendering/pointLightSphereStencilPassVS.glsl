/** This shader is used for sphere representing 
  * the volume of influence of a point light in deferred rendering */
layout (location = 0) in vec3 vPos;

layout (std140) uniform CommonMat {
    mat4 projection;
    mat4 view;
	mat4 projectionView;
};

layout (std140) uniform Lights {
    int numLights;
    Light lights[10];
};

uniform float scale;
uniform int lightIndex;

void main() {
    gl_Position = projectionView * vec4(lights[lightIndex].position + scale * vPos, 1.0);
}
