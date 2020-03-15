uniform mat4 _ssr_projectionView; // current projection view matrix
uniform vec3 _ssr_cameraPosition; // current camera position
uniform mat4 _ssr_view; // current camera direction

uniform sampler2D _ssr_position; // position data coming from deferred rendering
uniform sampler2D _ssr_normals;  // normals coming from deferred rendering
uniform sampler2D _ssr_specular;

const float _SSR_RAY_LENGTH = 3;
const int _SSR_RAY_ITERATIONS = 250;

vec4 ssr(vec4 color) {
	const vec2 screenSize = textureSize(_ssr_position, 0);

	const vec3 position = texture(_ssr_position, texCoord).xyz;
	const vec3 normal = texture(_ssr_normals, texCoord).xyz;
	const float shininess = texture(_ssr_specular, texCoord).a / 320;

	/* Dirty trick to avoid applying this effect to fragment
	 * for which this information is not existent. That is,
	 * fragments not rendered using deferred rendering */
	if (normal == vec3(0.0)) {
		return color;
	}

	const vec3 rayFromCamera = position - _ssr_cameraPosition;
	const vec3 rayDirection = reflect(normalize(rayFromCamera), normal);

	int found = 0;
	vec4 reflected = vec4(0, 0, 0, 0);
	for (int i = 1; i < _SSR_RAY_ITERATIONS; ++i) {
		// computes the ray position for this iteration
		const vec3 rayPosition = position + rayDirection * _SSR_RAY_LENGTH * i;

		// projects the ray onto the screen
		vec4 projected = _ssr_projectionView * vec4(rayPosition, 1.0);
		projected.xyz /= projected.w;

		// Gets the screen space coordinates of the ray (from ndc to ss)
		const vec2 rayScreenSpacePosition = projected.xy * 0.5 + 0.5;
		
		// depth of the ray
		const float rayDepth = projected.z * 0.5 +  0.5;

		// TODO TEMP
		if (any(lessThan(rayScreenSpacePosition, vec2(0.01))) || any(greaterThan(rayScreenSpacePosition, vec2(0.99)))) {
			break;
		}

		// The actual position of scene geometry where the ray is
		const vec3 geometryPosition = texture(_ssr_position, rayScreenSpacePosition).xyz;
		const float geometryDepth = texture(depthTexture, rayScreenSpacePosition).r;
		if (geometryDepth < rayDepth && distance(rayPosition, geometryPosition) < 3.5) {
			reflected = texture(screenTexture, rayScreenSpacePosition);
			found = 1;
			break;
		}
	}

	//return vec4(vec3(shininess), 1.0);
	return vec4(mix(color, reflected, shininess * found));
}
