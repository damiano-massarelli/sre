uniform mat4 _ssr_projectionView; // current projection view matrix
uniform vec3 _ssr_cameraPosition; // current camera position in world space
uniform vec3 _ssr_cameraDirection; // current camera direction
uniform vec4 _ssr_frustumPlanes[6]; // list of planes of the camera frustm

uniform sampler2D _ssr_position; // position data coming from deferred rendering
uniform sampler2D _ssr_normals;  // normals coming from deferred rendering
uniform sampler2D _ssr_materialBuffer;
uniform sampler2D _ssr_blurredScene;

uniform float _ssr_rayMaxDistance;
uniform float _ssr_rayResolution; // 1 to procede every pixel, within 0 and 1 to sample further
uniform int _ssr_raySteps;
uniform float _ssr_rayHitThreshold;

uniform float _ssr_nearPlane;
uniform float _ssr_farPlane;

vec3 _ssr_rayFrustumIntersection(vec3 rayStart, vec3 rayDirection, float rayLength) {
	for (int i = 0; i < 6; ++i) {
		float denom = dot(rayDirection, _ssr_frustumPlanes[i].xyz);
		if (abs(denom) > 0.01) {
			float num = dot(vec4(-rayStart, 1.), _ssr_frustumPlanes[i]);
			float t = num / denom;
			if (t > 0 && t < rayLength) {
				rayLength = t;
			}
		}
	}

	return rayStart + rayDirection * rayLength;
} 

float _ssr_viewSpaceDepthAt(vec2 coord) {
    float z = texture(depthTexture, coord).r;
	z = 2.0 * z - 1.0;
    float d = 2.0 * _ssr_nearPlane * _ssr_farPlane / (_ssr_farPlane + _ssr_nearPlane - z * (_ssr_farPlane - _ssr_nearPlane));

    return d;
}

vec4 ssr(vec4 color) {
	const vec2 screenSize = textureSize(_ssr_position, 0);

	const vec3 fragmentWorldPosition = texture(_ssr_position, texCoord).xyz;
	vec3 worldNormal = texture(_ssr_normals, texCoord).xyz;

	// Dirty trick to skip fragments that are not rendered into the GBuffer
	if (worldNormal == vec3(0.0)) {
		return color;
	}

	// Now we can safely normalize
	worldNormal = normalize(worldNormal);

	float biasedThreshold = max(_ssr_rayHitThreshold, 4.0 * _ssr_rayHitThreshold * (1.0 - dot(-_ssr_cameraDirection, worldNormal)));

	const vec3 viewDirection = normalize(fragmentWorldPosition - _ssr_cameraPosition);
	const vec3 rayDirection = normalize(reflect(viewDirection, worldNormal));

	vec3 rayStart = fragmentWorldPosition.xyz;
	vec3 rayEnd = _ssr_rayFrustumIntersection(rayStart, rayDirection, _ssr_rayMaxDistance);
	
	float rayStartViewSpaceDepth = dot(rayStart - _ssr_cameraPosition, _ssr_cameraDirection);
	float rayEndViewSpaceDepth = dot(rayEnd - _ssr_cameraPosition, _ssr_cameraDirection);
	
	// Project the start point from world to screen space
    vec4 rayStartScreenSpace = _ssr_projectionView * vec4(rayStart, 1.0);
	rayStartScreenSpace.xyz /= rayStartScreenSpace.w;
	rayStartScreenSpace.xyz = rayStartScreenSpace.xyz * 0.5 + 0.5;

	// Project the end point from world to screen space
	vec4 rayEndScreenSpace = _ssr_projectionView * vec4(rayEnd, 1.0);
	rayEndScreenSpace.xyz /= rayEndScreenSpace.w;
	rayEndScreenSpace.xyz = rayEndScreenSpace.xyz * 0.5 + 0.5;

	// Convert the UV coordinates to fragment/pixel coordinates
	vec2 startFrag = rayStartScreenSpace.xy * screenSize;
	vec2 endFrag   = rayEndScreenSpace.xy * screenSize;

	float deltaX = endFrag.x - startFrag.x;
  	float deltaY = endFrag.y - startFrag.y;

	// Make sure that we move along rayDirection on the longest side between 
	// deltaX and deltaY by a pixel at a time (if resolution is 1)
	float delta = max(abs(deltaX), abs(deltaY)) * clamp(_ssr_rayResolution, 0, 1);
  	vec2 increment = vec2(deltaX, deltaY) / max(delta, 0.01);

	// Percentage of the sample previous the hit
	float search0 = 0;
	// Percentage of movement
	float search1 = 0;

	// Whether or not we hit something
	int hit = 0;

	float rayDistanceViewSpace = 0.0;
	float depth = 0.0;
	float depthDelta = 0.0; // difference between ray depth and scene depth

	vec2 uv = vec2(0.0);

	vec2 frag = startFrag;
	
	// Step 1: search for hits of the reflected ray
	for (float i = 0; i < int(delta); ++i) {
		frag += increment;
		uv = frag / screenSize;
		search1 = i / delta;

		depth = _ssr_viewSpaceDepthAt(uv);
		rayDistanceViewSpace = (rayStartViewSpaceDepth * rayEndViewSpaceDepth) / mix(rayEndViewSpaceDepth, rayStartViewSpaceDepth, search1);
		depthDelta = rayDistanceViewSpace - depth;

		if (depthDelta > 0 && depthDelta < biasedThreshold) {
			hit = 1;
			break;
		} else {
			search0 = search1;
		}
	}

	// No hit
	if (hit == 0) {
		return color;
	}

	// Step 2: do a "binary" search midway the start and the hit point
	search1 = search0 + ((search1 - search0) / 2.0);

	for (int i = 0; i < _ssr_raySteps; ++i) {
    	frag = mix(startFrag, endFrag, search1);
    	uv = frag / screenSize;
    			
		rayDistanceViewSpace = (rayStartViewSpaceDepth * rayEndViewSpaceDepth) / mix(rayEndViewSpaceDepth, rayStartViewSpaceDepth, search1);
		depth = _ssr_viewSpaceDepthAt(uv);
		depthDelta = rayDistanceViewSpace - depth;
		if (depthDelta > 0 && depthDelta < biasedThreshold) {
			hit = 1;
			search1 = search0 + ((search1 - search0) / 2.0);
		} else {
			float temp = search1;
			search1 = search1 + ((search1 - search0) / 2.0);
			search0 = temp;
		}
  	}

	uv = mix(startFrag, endFrag, search1).xy / screenSize;

	vec2 uvOutOfBounds = vec2(1.0) - smoothstep(vec2(0.85), vec2(1.0), uv);
	uvOutOfBounds     *= 1.0 - smoothstep(vec2(0.85), vec2(1.0), 1.0 - uv);
	float zOutOfBounds = smoothstep(0.0, 0.02, (rayEndViewSpaceDepth - _ssr_nearPlane) / _ssr_farPlane);

	// How much the reflection vector point to the same direction of the view
	float reflectionCloseToViewDirection = (1 - max(dot(viewDirection, -rayDirection), 0));

	// How far is reflection point compared to the respective step
	float furtherFromHit = (1.0 - clamp(depthDelta / biasedThreshold, 0.0, 1.0));

	// How far the reflection point is compared to the starting point
	float furtherFromFirstRefl = 1.0 - distance(rayStart, rayStart + rayDirection * _ssr_rayMaxDistance * search1) / _ssr_rayMaxDistance;

	// Whether the hit point is too close to the start point or not
	float selfHit = smoothstep(0., 0.01, distance(rayStart, rayStart + rayDirection * _ssr_rayMaxDistance * search1) / _ssr_rayMaxDistance);

	float attenuation = reflectionCloseToViewDirection * furtherFromHit * furtherFromFirstRefl;

	float roughness = texture(_ssr_materialBuffer, texCoord).x;
	const int MAX_LOD = 5;
	vec4 reflectedColor = texture(screenTexture, uv);
	vec4 blurredReflectedColor = texture(_ssr_blurredScene, uv);
	reflectedColor = mix(reflectedColor, blurredReflectedColor, roughness);

	reflectedColor = mix(vec4(0.0), reflectedColor, selfHit * attenuation * hit * uvOutOfBounds.x * uvOutOfBounds.y * zOutOfBounds);
	
	return color + reflectedColor;
}
