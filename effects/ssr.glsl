uniform mat4 _ssr_projectionView; // current projection view matrix
uniform vec3 _ssr_cameraPosition; // current camera position in world space
uniform vec3 _ssr_cameraDirection; // current camera direction

uniform sampler2D _ssr_position; // position data coming from deferred rendering
uniform sampler2D _ssr_normals;  // normals coming from deferred rendering
uniform sampler2D _ssr_specular;

uniform float _ssr_ray_max_distance;
uniform float _ssr_ray_resolution; // 1 to procede every pixel, within 0 and 1 to sample further
uniform int _ssr_ray_steps;
uniform float _ssr_ray_hit_threshold;


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

	const vec3 viewDirection = normalize(fragmentWorldPosition - _ssr_cameraPosition);
	const vec3 rayDirection = normalize(reflect(viewDirection, worldNormal));

	vec3 rayStart = fragmentWorldPosition.xyz;
	vec3 rayEnd = rayStart + (rayDirection * _ssr_ray_max_distance);
	
	float rayStartViewSpaceDepth = dot(rayStart - _ssr_cameraPosition, _ssr_cameraDirection);
	float rayEndViewSpaceDepth = dot(rayEnd - _ssr_cameraPosition, _ssr_cameraDirection);
	
	// Project the start point from world to screen space
    vec4 rayStartScreenSpace = _ssr_projectionView * vec4(rayStart, 1.0);
	rayStartScreenSpace.xyz /= rayStartScreenSpace.w;
	rayStartScreenSpace.xy = rayStartScreenSpace.xy * 0.5 + 0.5;

	// Project the end point from world to screen space
	vec4 rayEndScreenSpace = _ssr_projectionView * vec4(rayEnd, 1.0);
	rayEndScreenSpace.xyz /= rayEndScreenSpace.w;
	rayEndScreenSpace.xy = rayEndScreenSpace.xy * 0.5 + 0.5;
	if (rayEndScreenSpace.z < 0.0 || rayEndScreenSpace.z > 1.0) {
		return color;
	}

	// Convert the UV coordinates to fragment/pixel coordinates
	vec2 startFrag = rayStartScreenSpace.xy * screenSize;
	vec2 endFrag = rayEndScreenSpace.xy * screenSize;
	endFrag = clamp(endFrag, vec2(0.0), screenSize); // There is not benefit going outside the screen, there is nothing to sample there

	float deltaX = endFrag.x - startFrag.x;
  	float deltaY = endFrag.y - startFrag.y;

	// Make sure that we move along rayDirection on the longest side between 
	// deltaX and deltaY by a pixel at a time (if resolution is 1)
	float delta = max(abs(deltaX), abs(deltaY)) * clamp(_ssr_ray_resolution, 0, 1);
  	vec2 increment = vec2(deltaX, deltaY) / max(delta, 0.01);

	// Percentage of the sample previous the hit
	float search0 = 0;
	// Percentage of movement
	float search1 = 0;

	// Whether or not we hit something
	int hit = 0;

	float viewDistance = 0;
	float depth = 0;

	vec2 uv = vec2(0.0);
	vec3 testPosition = vec3(0.0);

	vec2 frag = startFrag;
	
	for (float i = 0; i < int(delta); ++i) {
		frag += increment;
		uv = frag / screenSize;
		testPosition = texture(_ssr_position, uv).xyz;
		if (testPosition == vec3(0.0)) {
			continue;
		}

		search1 = i / int(delta);

		viewDistance = (rayStartViewSpaceDepth * rayEndViewSpaceDepth) / mix(rayEndViewSpaceDepth, rayStartViewSpaceDepth, search1);
		depth = viewDistance - dot(testPosition - _ssr_cameraPosition, _ssr_cameraDirection);

		if (depth > 0 && depth < _ssr_ray_hit_threshold) {
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

	// Reset the hit as we are going to search for it again
	//hit = 0;

	// Step 2: do a "binary" search midway the start and the hit point
	search1 = search0 + ((search1 - search0) / 2.0);

	for (int i = 0; i < _ssr_ray_steps; ++i) {
    	frag = mix(startFrag.xy, endFrag.xy, search1);
    	uv = frag / screenSize;
    	testPosition = texture(_ssr_position, uv).xyz;
		if (testPosition != vec3(0.0)) {

			viewDistance = (rayStartViewSpaceDepth * rayEndViewSpaceDepth) / mix(rayEndViewSpaceDepth, rayStartViewSpaceDepth, search1);
			depth = viewDistance - dot(testPosition - _ssr_cameraPosition, _ssr_cameraDirection);

			if (depth > 0 && depth < _ssr_ray_hit_threshold) {
				hit = 1;
				search1 = search0 + ((search1 - search0) / 2.0);
			} else {
				float temp = search1;
				search1 = search1 + ((search1 - search0) / 2.0);
				search0 = temp;
			}
		}
  	}

	// How much the reflection vector point to the same direction of the view
	//float reflectionCloseToViewDirection = (1 - max(dot(viewDirection, -rayDirection), 0)); // His version
	float reflectionCloseToViewDirection = (1 - abs(dot(viewDirection, -rayDirection))); // My version

	// How far is reflection point compared to the respective step
	float furtherFromHit = (1 - clamp(depth / _ssr_ray_hit_threshold, 0, 1));

	// How far the reflection point is compared to the starting point
	float furtherFromFirstRefl = (1 - clamp(length(testPosition - fragmentWorldPosition) / _ssr_ray_max_distance, 0, 1));

	float attenuation = reflectionCloseToViewDirection * furtherFromHit * furtherFromFirstRefl;

	//return mix(color, vec4(hit, 0.0, 0.0, 1.0), hit) * attenuation;
	return mix(color, vec4(hit, 0.0, 0.0, 1.0), attenuation);
}
