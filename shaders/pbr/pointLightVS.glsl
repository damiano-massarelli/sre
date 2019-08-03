/** This shader is used for sphere representing 
  * the volume of influence of a point light in deferred rendering (pbr) */
layout (location = 0) in vec2 vPos;

void main() {
    gl_Position = vec4(vPos, 0.0, 1.0);
}
