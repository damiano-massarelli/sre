uniform samplerCube cubemap;

in vec3 position;

out vec4 FragColor;

uniform float lowerFogLimit = 0.0;
uniform float upperFogLimit = 0.1;

void main() {
    vec4 color = texture(cubemap, position);
	//color.rgb = pow(color.rgb, vec3(2.2f)); // gamma correction

	float fogFactor = (position.y - lowerFogLimit) / (upperFogLimit - lowerFogLimit);
	fogFactor = clamp(fogFactor, 0.0, 1.0);

	FragColor = mix(vec4(_fogColor, 1.0), color, fogFactor);
}
