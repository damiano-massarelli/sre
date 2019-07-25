/**
  * Requires that a sampler2D named shadowMap is available */

uniform samplerCube shadowCube;
uniform float lightRadius;

const float BIAS = 0.05;

float pointMapIsInShadow(vec3 position, vec3 lightPosition) {
	
	vec3 sampleRay = position - lightPosition; 
    float closestDepth = texture(shadowCube, sampleRay).r * lightRadius;

	float currentDepth = length(sampleRay);

	float shadow = currentDepth -  BIAS > closestDepth ? 1.0 : 0.0;
	return shadow;
}
