layout (location = 0) out vec4 FragColor;

uniform samplerCube cubemap;

in vec3 position;

uniform float lowerFogLimit = 0.0;
uniform float upperFogLimit = 0.1;

const vec3 _fogColor = vec3(0.51, 0.59, 0.67);


void main() {
    vec3 color = texture(cubemap, position).rgb;

	float fogFactor = (position.y - lowerFogLimit) / (upperFogLimit - lowerFogLimit);
	fogFactor = clamp(fogFactor, 0.0, 1.0);

	vec3 finalColor = mix(_fogColor, color, fogFactor);
	finalColor = pow(finalColor, vec3(2.2));

	FragColor = vec4(color, 1.0);
}
