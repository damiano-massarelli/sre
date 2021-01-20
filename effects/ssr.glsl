uniform sampler2D _ssr_reflectionTexture;
const int _SSR_MAX_LOD = 6;

vec4 ssr(vec4 color) {
	float roughness = texture(_ssr_reflectionTexture, texCoord).a;
	vec3 reflectionColor = textureLod(_ssr_reflectionTexture, texCoord, roughness * _SSR_MAX_LOD).rgb;

	vec3 resultColor = reflectionColor * (1. - roughness);
	return color + vec4(resultColor, 0.);
}