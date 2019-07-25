in vec4 position;

uniform vec3 lightPos;
uniform float farPlane; // aka the radius of the light

void main()
{
    // distance to light
    float distanceToLight = length(position.xyz - lightPos);
    
    distanceToLight = distanceToLight / farPlane;
    
    gl_FragDepth = 0.5;
} 