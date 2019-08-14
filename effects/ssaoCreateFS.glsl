
in vec2 texCoord;
out float FragColor;

uniform int kernelSize = 12;
uniform float radius = 2.5;

const float noiseResolution = 4.0f; // see res in SSAO::createNoise
const float minBias = 0.05;
const float maxBias = 0.2;

layout (std140) uniform CommonMat {
    mat4 projection;
    mat4 view;
	mat4 projectionView;
	vec4 clipPlane;
};

layout (std140) uniform Camera {
    vec3 cameraPosition;
    vec3 cameraDirection;
};

uniform sampler2D src; 			// the position data
uniform sampler2D NormalData; 
uniform sampler2D noise;

uniform vec3 samples[64];

void main() {
	vec2 screenSize = textureSize(src, 0);
	vec2 tiledTexCoord = texCoord * (screenSize / noiseResolution);

	vec3 position = texture(src, texCoord).xyz;
	vec3 normal = texture(NormalData, texCoord).xyz;

	/* Dirty trick to avoid applying this effect to fragment
	 * for which this information is not existent. That is,
	 * fragments not rendered using deferred rendering */
	if (normal == vec3(0.0)) discard;

	vec3 randomVec = texture(noise, tiledTexCoord).xyz;

	vec3 tangent = normalize(randomVec - normal * dot(normal, randomVec));
	vec3 bitangent = cross(normal, tangent);

	mat3 tangentToWorld = mat3(tangent, bitangent, normal);

	float occlusion = 0.0;

	vec3 rayToCamera = normalize(cameraPosition - position);
	float bias = mix(minBias, maxBias, 1 - max(dot(rayToCamera, normal), 0.0));

	for (int i = 0; i < kernelSize; ++i) {
		// from tangent to world space
		vec3 samp = tangentToWorld * samples[i];
		samp = position + samp * radius;

		float sampleZ = dot(samp - cameraPosition, cameraDirection);

		// project the sampe
		vec4 projected = projectionView * vec4(samp, 1.0);
		projected.xy /= projected.w;
		projected.xy = projected.xy * 0.5 + 0.5;
		projected.xy = clamp(projected.xy, vec2(0.001), vec2(0.999));

		// get the z value where the sample lies
		vec3 toOriginal = texture(src, projected.xy).xyz - cameraPosition;
		float originalZ = dot(cameraDirection, toOriginal);

		// add 1 to penetration if the sampleDepth is greater then sample.z
		// which would mean that sample is inside some object
		float penetration = (sampleZ >= originalZ + bias ? 1.0 : 0.0);

		float rangeCheck = abs(sampleZ - originalZ) > radius ? 0.0 : 1.0; 

		// scale penetration
		occlusion += penetration * rangeCheck;
	}

	FragColor = occlusion / kernelSize;
}
