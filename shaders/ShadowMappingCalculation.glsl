/**
  * Requires that a sampler2D named shadowMap is available */

uniform sampler2D shadowMap;

layout (std140) uniform ShadowMapParams {
    vec4 _shadowParams; // x: distance, y: fade out range, z: shadow strength, w: outside shadow map (shadow or light?)
};

const float SHADOWMAP_MIN_BIAS = 0.0001;
const float SHADOWMAP_MAX_BIAS = 0.001;

const int SMOOTH_RANGE = 1;

/**
  * Return 1 if the fragment is in shadow, 0 otherwise.
  * @param lightSpacePos the position of the current fragment in light space
  * @param lightDirection the direction of the light casting shadows (world space)
  * @param the normal of the current fragment (world space)
  * @param distance the distance of the current fragment from the camera
  * @return 1 if in shadow, 0 otherwise */
float shadowMapIsInShadow(vec4 lightSpacePos, vec3 lightDirection, vec3 normal, float dist) {
	float shadowStrength = _shadowParams[2];
	if (shadowStrength == 0.0)
		return 0.0f;

	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

	vec3 shadowSampleCoord = lightSpacePos.xyz / lightSpacePos.w;
	shadowSampleCoord = (shadowSampleCoord + vec3(1.0)) / 2.0;

	// if we are out of the shadow map texture just return 1
	if (any(lessThan(shadowSampleCoord, vec3(0))) || any(greaterThan(shadowSampleCoord, vec3(1))))
		return 0.0;

	float bias = max(SHADOWMAP_MIN_BIAS, SHADOWMAP_MAX_BIAS * (1.0 - dot(lightDirection, normal)));

	float inShadow = 0.0;
	for (int i = -SMOOTH_RANGE; i <= SMOOTH_RANGE; i++) {
		for (int j = -SMOOTH_RANGE; j <= SMOOTH_RANGE; j++) {
			float depthInShadowMap = texture(shadowMap, shadowSampleCoord.xy + texelSize * vec2(i, j)).r;
			inShadow += float(shadowSampleCoord.z - bias > depthInShadowMap);
		}
	}

	const float shadowDistance = _shadowParams.x;
	const float fadeOutRange = _shadowParams.y;

	float disappearFactor = (dist - shadowDistance + fadeOutRange) / fadeOutRange;
	disappearFactor = clamp(disappearFactor, 0.0, 1.0);


	return shadowStrength * (1 - disappearFactor) * inShadow / ((2.0 * SMOOTH_RANGE + 1.0) * (2.0 * SMOOTH_RANGE + 1.0));
}
