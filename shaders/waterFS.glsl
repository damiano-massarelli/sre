out vec4 FragColor;

in vec4 clipSpaceCoord;
in vec2 texCoord;
in vec3 position;

uniform float moveDuDv;

uniform sampler2D reflection;
uniform sampler2D refraction;
uniform sampler2D dudv;

const float dudvTiles = 6.0;

const float waveStrength = 0.02;

layout (std140) uniform Camera {
    vec3 cameraPosition;
    vec3 cameraDirection;
};

void main() {
	vec2 ndcTexCoord = (clipSpaceCoord.xy / clipSpaceCoord.w) / 2 + 0.5;
	vec2 reflectionTexCoord = vec2(ndcTexCoord.x, -ndcTexCoord.y);
	vec2 refractionTexCoord = ndcTexCoord;

	vec2 tiledTexCoord = texCoord * dudvTiles;
	vec2 distortion1 = (texture(dudv, vec2(tiledTexCoord.x + moveDuDv, tiledTexCoord.y)).xy * 2 - 1) * waveStrength;
	vec2 distortion2 = (texture(dudv, vec2(-tiledTexCoord.x + moveDuDv, tiledTexCoord.y + moveDuDv)).xy * 2 - 1) * waveStrength;
	vec2 totDistortion = distortion1 + distortion2;

	reflectionTexCoord += totDistortion;
	reflectionTexCoord.x = clamp(reflectionTexCoord.x, 0.001, 0.999);
	reflectionTexCoord.y = clamp(reflectionTexCoord.y, -0.999, -0.001);

	refractionTexCoord += totDistortion;
	refractionTexCoord = clamp(refractionTexCoord, 0.001, 0.999);

	vec4 reflectionColor = texture(reflection, reflectionTexCoord);
	vec4 refractionColor = texture(refraction, refractionTexCoord);

	// Fresnel effect
	vec3 rayToCamera = normalize(cameraPosition - position);
	float fresnelFactor = dot(rayToCamera, vec3(0, 1, 0));
	fresnelFactor = pow(fresnelFactor, 0.5);

    FragColor = mix(reflectionColor, refractionColor, fresnelFactor);
	FragColor = mix(FragColor, vec4(0.0, 0.3, 0.3, 1.0), 0.2);
}