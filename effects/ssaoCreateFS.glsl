
in vec2 texCoord;
out float FragColor;

const float noiseResolution = 4.0f; // see res in SSAO::createNoise
const int kernelSize = 64;
const float radius = 0.5;
const float bias = 0.025;

layout (std140) uniform CommonMat {
    mat4 projection;
    mat4 view;
	mat4 projectionView;
	vec4 clipPlane;
};


uniform sampler2D src; 			// the position data
uniform sampler2D NormalData; 
uniform sampler2D noise;

uniform vec3 samples[64];

void main() {
	vec2 screenSize = textureSize(src, 0);
	vec2 tiledTexCoord = texCoord * (screenSize / noiseResolution);

	vec3 position = texture(src, texCoord).xyz;
	if (position.z == 0.0) discard;
	vec3 normal = texture(NormalData, texCoord).xyz;
	vec3 randomVec = texture(noise, tiledTexCoord).xyz;

	vec3 tangent = normalize(randomVec - normal * dot(normal, randomVec));
	vec3 bitangent = cross(normal, tangent);

	mat3 tangentToWorld = mat3(tangent, bitangent, normal);

	float occlusion = 0.0;
	float sampleDepth = 0.0;
	for (int i = 0; i < kernelSize; ++i) {
		// from tangent to world space
		vec3 samp = tangentToWorld * samples[i];
		samp = position + samp * radius;

		vec4 sampView = view * vec4(samp, 1.0);

		// project the sampe
		vec4 projected = projection * sampView;
		projected.xy /= projected.w;
		projected.xy = projected.xy * 0.5 + 0.5;
		projected.xy = clamp(projected.xy, vec2(0.001), vec2(0.999));

		// get the z value where the sample lies
		vec4 sampled = view * vec4(texture(src, projected.xy).rgb, 1.0);
		sampleDepth = sampled.z;

		// add 1 to penetration if the sampleDepth is greater then sample.z
		// which would mean that sample is inside some object
		float penetration = (sampleDepth >= sampView.z + bias ? 1.0 : 0.0);

		float rangeCheck = smoothstep(0.0, 1.0, radius / (abs(position.z - sampleDepth))); 

		// scale penetration
		occlusion += penetration;// * rangeCheck;
	}

	FragColor = occlusion / kernelSize;
}
