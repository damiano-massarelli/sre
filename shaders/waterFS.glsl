out vec4 FragColor;

in vec4 clipSpaceCoord;
in vec2 texCoord;

uniform float moveDuDv;

uniform sampler2D reflection;
uniform sampler2D refraction;
uniform sampler2D dudv;

const float dudvTiles = 3.0;

const float waveStrength = 0.02;

void main() {
	vec2 ndcTexCoord = (clipSpaceCoord.xy / clipSpaceCoord.w) / 2 + 0.5;

	vec2 distortion1 = (texture(dudv, texCoord * dudvTiles).xy * 2 - 1) * waveStrength;
	ndcTexCoord += distortion1;

	vec2 reflectionTexCoord = vec2(ndcTexCoord.x, -ndcTexCoord.y);
	reflectionTexCoord.x = clamp(reflectionTexCoord.x, 0.001, 0.999);
	reflectionTexCoord.y = clamp(reflectionTexCoord.y, -0.999, -0.001);

	vec2 refractionTexCoord = ndcTexCoord;
	refractionTexCoord = clamp(refractionTexCoord, 0.001, 0.999);

	vec4 reflectionColor = texture(reflection, reflectionTexCoord);
	vec4 refractionColor = texture(refraction, refractionTexCoord);
    FragColor = mix(reflectionColor, refractionColor, 0.5);
}