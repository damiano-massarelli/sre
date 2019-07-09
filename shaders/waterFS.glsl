layout (location = 0) out vec4 Diffuse;
layout (location = 1) out vec4 Specular;
layout (location = 2) out vec3 Position;
layout (location = 3) out vec3 Normal;

in vec4 clipSpaceCoord;
in vec2 texCoord;
in vec3 position;

uniform float moveDuDv;

uniform sampler2D reflection;
uniform sampler2D refraction;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform sampler2D groundDiffuseMap;
uniform sampler2D groundSpecularMap;
uniform sampler2D groundNormalMap;

uniform float far;
uniform float near;

const float dudvTiles = 6.0;

const float waveStrength = 0.02;

layout (std140) uniform Camera {
    vec3 cameraPosition;
    vec3 cameraDirection;
};

void main() {
	// calculate the texture coordinate for the ndc coordinates of this fragment
	vec2 ndcTexCoord = (clipSpaceCoord.xy / clipSpaceCoord.w) / 2 + 0.5;
	vec2 reflectionTexCoord = vec2(ndcTexCoord.x, -ndcTexCoord.y);
	vec2 refractionTexCoord = ndcTexCoord;

	// get color, specular and normal of the underlying ground (will be mixed with those of the water)
	vec3 groundNormal = texture(groundNormalMap, ndcTexCoord).rgb;
	vec4 groundSpecular = texture(groundSpecularMap, ndcTexCoord);
	vec4  groundDiffuse = texture(groundDiffuseMap, ndcTexCoord);

	// get ground and water height and compute the depth of the water
	float height = texture(depthMap, ndcTexCoord).r;
	float groundHeight = 2.0 * near * far / (far + near - (2.0 * height - 1.0) * (far - near));
	height = 2.0 * near * far / (far + near - (2.0 * gl_FragCoord.z - 1.0) * (far - near));
	float waterDepth = groundHeight - height;
	waterDepth = clamp(waterDepth / 5.0, 0, 1);

	// tile texture coordinated
	vec2 tiledTexCoord = texCoord * dudvTiles;

	// distort texture coordinates based on dudv map
	vec2 distortedTexCoords = texture(dudvMap, vec2(tiledTexCoord.x + moveDuDv, tiledTexCoord.y)).rg * 0.1;
	distortedTexCoords = tiledTexCoord + vec2(distortedTexCoords.x, distortedTexCoords.y + moveDuDv);
	vec2 totDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength * waterDepth;


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

	// compute diffuse color and mix it with ground diffuse
    Diffuse = mix(reflectionColor, refractionColor, fresnelFactor);
	Diffuse = mix(Diffuse, vec4(0.0, 0.3, 0.3, 1.0), 0.2); // add blue-ish tint
	Diffuse = mix(groundDiffuse, Diffuse, waterDepth);

	// compute specular color and mix it with ground specular
	Specular = vec4(vec3(0.5) * waterDepth, 128.0);
	Specular = mix(groundSpecular, Specular, waterDepth);
	Specular.a *= float(groundSpecular.a != 0 || waterDepth == 1.0); // if the ground is a lambert keep it that way

	// compute normal and mix it with that of the ground
	vec4 sampledNormal = texture(normalMap, distortedTexCoords);
	// the z component is rotated into the y component
	vec3 normal = vec3(sampledNormal.r * 2 - 1, sampledNormal.b, (sampledNormal.g * 2 - 1));
	normal = normalize(normal + vec3(0, 5, 0));

	Normal = mix(groundNormal, normal, waterDepth);

	// position is not mixed with that of the ground since it is not
	// beneficial
	Position = position;
}