/**
  * Requires that a sampler2D named shadowMap is available */

uniform sampler2D shadowMap;

const float SHADOWMAP_MIN_BIAS = 0.0001;
const float SHADOWMAP_MAX_BIAS = 0.0005;

const int RANGE = 1;

/**
  * Return 1 if the fragment is in shadow, 0 otherwise.
  * @param lightSpacePos the position of the current fragment in light space
  * @param lightDirection the direction of the light casting shadows (world space)
  * @param the normal of the current fragment (world space)
  * @return 1 if in shadow, 0 otherwise */
float shadowMapIsInShadow(vec4 lightSpacePos, vec3 lightDirection,  vec3 normal) {
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

	vec3 shadowSampleCoord = lightSpacePos.xyz / lightSpacePos.w;
	shadowSampleCoord = (shadowSampleCoord + vec3(1.0)) / 2.0;

	// if we are out of the shadow map texture just return 1
	if (any(lessThan(shadowSampleCoord, vec3(0))) || any(greaterThan(shadowSampleCoord, vec3(1))))
		return 0.0f;


	float bias = max(SHADOWMAP_MIN_BIAS, SHADOWMAP_MAX_BIAS * (1.0 - dot(lightDirection, normal)));

	float inShadow = 0.0;
	for (int i = -RANGE; i <= RANGE; i++) {
		for (int j = -RANGE; j <= RANGE; j++) {
			// TODO check the value of w to see if
			// perspective projection is being used. If that's the case linearize depth.
			float depthInShadowMap = texture(shadowMap, shadowSampleCoord.xy + texelSize * vec2(i, j)).r;
			inShadow += float(shadowSampleCoord.z - bias > depthInShadowMap);
		}
	}

	return inShadow / ((2.0 * RANGE + 1.0) * (2.0 * RANGE + 1.0));
}
