/**
  * Requires that a samplerCube named shadowCube is available */

uniform samplerCube shadowCube;
uniform float lightRadius;

const float BIAS = 0.1;

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);  

float pointMapIsInShadow(vec3 position, vec3 lightPosition, vec3 cameraPosition) {
	
	vec3 sampleRay = position - lightPosition; 

	float currentDepth = length(sampleRay);
	float shadow = 0.0;

	float pcfRadius = (1.0 + length(position - cameraPosition) / lightRadius) / 25.0;

	for (int i = 0; i < 20; i++) {
		float closestDepth = texture(shadowCube, sampleRay + sampleOffsetDirections[i] * pcfRadius).r * lightRadius;
		
		shadow += currentDepth -  BIAS > closestDepth ? 1.0 : 0.0;
	}

	shadow /= 20.0;

	return shadow;
}
