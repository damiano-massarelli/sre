layout (location = 0) out vec3 FragColor;

uniform samplerCube cubemap;

in vec3 position;

uniform float lowerFogLimit = 0.0;
uniform float upperFogLimit = 0.1;

void main() {
    vec3 color = texture(cubemap, position).rgb;

	float fogFactor = (position.y - lowerFogLimit) / (upperFogLimit - lowerFogLimit);
	fogFactor = clamp(fogFactor, 0.0, 1.0);

	
	FragColor = mix(_fogColor, color, fogFactor);
}
