// Uniforms
uniform mat4 _ssr_projectionView; // current projection view matrix
uniform vec3 _ssr_cameraPosition; // current camera position in world space
uniform vec3 _ssr_cameraDirection; // current camera direction
uniform vec4 _ssr_frustumPlanes[6]; // list of planes of the camera frustm

uniform sampler2D _ssr_position; // position data coming from deferred rendering
uniform sampler2D _ssr_normals;  // normals coming from deferred rendering
uniform sampler2D _ssr_materialBuffer;
uniform sampler2D _ssr_diffuseColor;

uniform float _ssr_rayMaxDistance;
uniform int _ssr_numSamples;
uniform int _ssr_raySteps;
uniform float _ssr_rayHitThreshold;
uniform float _ssr_steepAngleHitThresholdMultiplier;

uniform float _ssr_nearPlane;
uniform float _ssr_farPlane;

// Consts
const int _SSR_MAX_LOD = 6;
const float _SSR_MAX_ROUGHNESS = 0.9;
const float _SSR_SELF_HIT_INCREMENT = 0.75;

// Data Structures
struct RayHitInfo {
	// 1.0 if the ray hit something, 0.0 otherwise
	float hit;

	// UV coordinates of the hit point
	vec2 uvHitPoint;

	// Percentage between rayStart and rayEnd where the ray hit the scene
	float distanceTraveledPercentage;

	// Difference between the ray depth and scene depth
	float hitDepthDelta;

	// Camera space depth of the ray
	float hitRayDepth;
};

vec3 _ssr_fresnel(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
} 

// Returns the intersection between the ray originated in wsRayStart with direction
// wsRayDirection and length rayLength with the frustum planes. If no intersection occurs
// the function simply return wsRayStart + wsRayDirection * rayLength
vec3 _ssr_rayFrustumIntersection(vec3 wsRayStart, vec3 wsRayDirection, float rayLength) {
	for (int i = 0; i < 6; ++i) {
		float denom = dot(wsRayDirection, _ssr_frustumPlanes[i].xyz);
		if (abs(denom) > 0.01) {
			float num = dot(vec4(-wsRayStart, 1.), _ssr_frustumPlanes[i]);
			float t = num / denom;
			if (t > 0 && t < rayLength) {
				rayLength = t;
			}
		}
	}

	return wsRayStart + wsRayDirection * rayLength;
}

// Converts a non-linear depth value in [0, 1] to 
// a linear depth value in [_ssr_nearPlane, _ssr_farPlane]
float _ssr_toLinearDepth(float z) {
	z = 2.0 * z - 1.0;
    float d = 2.0 * _ssr_nearPlane * _ssr_farPlane / (_ssr_farPlane + _ssr_nearPlane - z * (_ssr_farPlane - _ssr_nearPlane));

    return d;
}

// Gets the linear camera space depth at the given uv coords.
float _ssr_viewSpaceDepthAt(vec2 coord) {
    float z = texture(depthTexture, coord).r;
	return _ssr_toLinearDepth(z);
}

float _ssr_getAttenuation(RayHitInfo hitInfo, vec3 wsRayStart, vec3 wsHitPoint, float csRayStartDepth, float csRayEndDepth, float hitThreshold) {
	vec2 uvOutOfBounds = vec2(1.0) - smoothstep(vec2(0.85), vec2(1.0), hitInfo.uvHitPoint);
	uvOutOfBounds     *= 1.0 - smoothstep(vec2(0.85), vec2(1.0), 1.0 - hitInfo.uvHitPoint);
	float zOutOfBounds = smoothstep(0.0, 0.02, (csRayEndDepth - _ssr_nearPlane) / _ssr_farPlane);

	// How far is reflection point compared to the respective step
	float furtherFromHit = 1. - smoothstep(hitInfo.hitDepthDelta / hitThreshold, 0., .5);

	// How far the reflection point is compared to the starting point
	float hitDistanceFromStart = distance(wsRayStart, wsHitPoint);
	float furtherFromFirstRefl = 1.0 - hitDistanceFromStart / _ssr_rayMaxDistance;

	// Whether the hit point is too close to the start point or not
	float selfHit = step(1. /_ssr_numSamples, hitInfo.distanceTraveledPercentage);

	float attenuation = hitInfo.hit * furtherFromHit * furtherFromFirstRefl * selfHit \
	                    * uvOutOfBounds.x * uvOutOfBounds.y * zOutOfBounds;

	return attenuation;
}

RayHitInfo _ssr_castRay(
	// ray start point uv coords
	vec2 ssRayStart,

	// ray start point depth
	float csRayStartDepth,
	
	// ray end point uv coords
	vec2 ssRayEnd,

	// ray end point depth
	float csRayEndDepth,

	// number of ray samples
	float numSamples,

	// number of binary search steps
	float binarySearchSteps) {

	// return data
	RayHitInfo hitInfo = RayHitInfo( 0., vec2(0.), 0., 0., 0. );

	// the following values contain uv coords in the xy comps. and the reciprocal of view space depth in the z comp.
	vec3 rayStart      = vec3(ssRayStart, 1. / csRayStartDepth);
	vec3 rayEnd        = vec3(ssRayEnd, 1. / csRayEndDepth);
	vec3 stepIncrement = (rayEnd - rayStart) / numSamples;

	// Percentage between wsRayStart and wsRayEnd at each step
	float stepPercentage = 1. / numSamples;

	// Step 1: search for intersection between reflected ray and scene
	for (float i = 1; i < numSamples; ++i) { // start with i = 1 to avoid self hit
		
		vec3 raySample = rayStart + stepIncrement * i;

		float sceneDepth = _ssr_viewSpaceDepthAt(raySample.xy);
		float rayDepth = 1. / raySample.z;
		float depthDelta = rayDepth - sceneDepth;
		if (depthDelta >= 0.) {

			// Binary search
			float distancePercentage = stepPercentage * i;
			float j = 0;
			do {
				stepIncrement /= 2.;

				if (depthDelta >= 0) {
					hitInfo.hit = 1.;
					hitInfo.uvHitPoint = raySample.xy;
					hitInfo.hitDepthDelta = depthDelta;
					hitInfo.distanceTraveledPercentage = distancePercentage;
					hitInfo.hitRayDepth = rayDepth;

					raySample -= stepIncrement;
					distancePercentage -= stepPercentage;
				}
				else {
					raySample += stepIncrement;
					distancePercentage += stepPercentage;	
				}

				sceneDepth = _ssr_viewSpaceDepthAt(raySample.xy);
				rayDepth = 1. / raySample.z;
		 		depthDelta = rayDepth - sceneDepth;
				stepPercentage /= 2;

				j++;
			} while (j < binarySearchSteps);
		}
	}

	return hitInfo;
}

vec4 ssr(vec4 color) {
	// Pre-fetch and check roughness
	vec3 materialData = texture(_ssr_materialBuffer, texCoord).xyz;
	float roughness = materialData[0];
	const float metalness = materialData[1];

	if (roughness > _SSR_MAX_ROUGHNESS) {
		return color;
	}
	roughness /= _SSR_MAX_ROUGHNESS;

	const vec3 wsFragmentPosition = texture(_ssr_position, texCoord).xyz;
	vec3 wsFragmentNormal = texture(_ssr_normals, texCoord).xyz;

	// Dirty trick to skip fragments that are not rendered into the GBuffer
	if (wsFragmentNormal == vec3(0.0)) {
		return color;
	}

	// Now we can safely normalize
	wsFragmentNormal = normalize(wsFragmentNormal);

	float biasedThreshold = _ssr_rayHitThreshold;// max(_ssr_rayHitThreshold, 4.0 * _ssr_rayHitThreshold * (1.0 - dot(-_ssr_cameraDirection, wsFragmentNormal)));

	const vec3 wsViewDirection = normalize(wsFragmentPosition - _ssr_cameraPosition);
	const vec3 wsRayDirection = normalize(reflect(wsViewDirection, wsFragmentNormal));

	vec3 wsRayStart = wsFragmentPosition.xyz;
	vec3 wsRayEnd = _ssr_rayFrustumIntersection(wsRayStart, wsRayDirection, _ssr_rayMaxDistance);

	// Prevent self hit if view direction is perpendicular to world normal
	const float VdotN = dot(-wsViewDirection, wsFragmentNormal);
	float selfHitIncrementFactor = mix(_SSR_SELF_HIT_INCREMENT, 0, VdotN);
	selfHitIncrementFactor = min(selfHitIncrementFactor, distance(wsRayStart, wsRayEnd));
	wsRayStart += wsRayDirection * max(selfHitIncrementFactor, 0);
		
	// Project the start point from world to screen space
    vec4 ssRayStart = _ssr_projectionView * vec4(wsRayStart, 1.0);
	ssRayStart.xyz /= ssRayStart.w;
	ssRayStart.xyz = ssRayStart.xyz * 0.5 + 0.5;
	float csRayStartDepth = _ssr_toLinearDepth(ssRayStart.z);

	// Project the end point from world to screen space
	vec4 ssRayEnd = _ssr_projectionView * vec4(wsRayEnd, 1.0);
	ssRayEnd.xyz /= ssRayEnd.w;
	ssRayEnd.xyz = ssRayEnd.xyz * 0.5 + 0.5;
	float csRayEndDepth = _ssr_toLinearDepth(ssRayEnd.z);

	RayHitInfo result = _ssr_castRay(ssRayStart.xy, csRayStartDepth,
									 ssRayEnd.xy, csRayEndDepth,
									 _ssr_numSamples, _ssr_raySteps);

	// Increase hit threshold if the ray is hitting a surface at a steep angle wrt the fragment-camera vector
	if (_ssr_steepAngleHitThresholdMultiplier > 1.) {
		vec3 wsHitNormal = texture(_ssr_normals, result.uvHitPoint).xyz;
		wsHitNormal = wsHitNormal != vec3(0.) ? normalize(wsHitNormal) : -wsViewDirection;

		float distanceMultiplier = clamp(result.hitRayDepth / (_ssr_farPlane / 3), 0., 1.);

		biasedThreshold = max(biasedThreshold, distanceMultiplier * _ssr_steepAngleHitThresholdMultiplier * biasedThreshold * (1. - dot(-wsViewDirection, wsHitNormal)));
	}

	// Fresnel
	vec3 F0 = vec3(0.04);
    F0      = mix(F0, texture(_ssr_diffuseColor, result.uvHitPoint).rgb, metalness);
    vec3 fresnel = _ssr_fresnel(max(dot(wsFragmentNormal, -wsViewDirection), 0.0), F0);

	const vec3 wsHitPoint = wsRayStart + wsRayDirection * _ssr_rayMaxDistance * result.distanceTraveledPercentage;
	const float attenuation = _ssr_getAttenuation(result, wsRayStart, wsHitPoint, csRayStartDepth, csRayEndDepth, biasedThreshold);
	vec4 reflectedColor = texelFetch(screenTexture, ivec2(textureSize(screenTexture, 0) * result.uvHitPoint), 0) * vec4(fresnel, 1.);

	reflectedColor = mix(vec4(0.0), reflectedColor, clamp(attenuation, 0, 1));
	
	return color + reflectedColor;
}
