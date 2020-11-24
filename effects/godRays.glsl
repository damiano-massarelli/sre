uniform int _gr_samples = 100;

uniform vec3 _gr_lightScreenPos;

uniform float _gr_radius;
uniform float _gr_density = 1.0;
uniform float _gr_decayRatio = 0.95;
uniform float _gr_weight = 0.9;
uniform vec3 _gr_lightColor;

vec4 godRays(vec4 color) {
    vec3 toneMappedLightColor = vec3(1.0) - exp(-_gr_lightColor);
    vec2 size = textureSize(screenTexture, 0);

    vec2 projectedLightPos = _gr_lightScreenPos.xy * 0.5 + 0.5;
    float projectedLightDepth = _gr_lightScreenPos.z * 0.5 + 0.5;

	vec2 deltaTexCoord = texCoord - projectedLightPos;
	deltaTexCoord *= (1.0 / float(_gr_samples)) * _gr_density;

    vec3 rayColor = vec3(0.0);

    float decay = 1.0;
    for (int i = 0; i < _gr_samples; i++) {
		vec2 coord = texCoord - deltaTexCoord * i;

        float depth = texture(depthTexture, coord).r;

	    vec2 dist = coord - projectedLightPos;
	    dist.x *= size.x / size.y;

        rayColor += vec3(toneMappedLightColor) * float(length(dist) < _gr_radius && depth > projectedLightDepth) * decay * _gr_weight;

        decay *= _gr_decayRatio;
	}

    color.rgb += rayColor;

    return color;
}
