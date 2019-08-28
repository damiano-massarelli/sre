
uniform sampler2D _gr_raysTexture;

uniform int _gr_samples = 100;

uniform vec3 _gr_lightScreenPos;

uniform float _gr_density = 1.0;
uniform float _gr_decayRatio = 0.95;
uniform float _gr_weight = 0.9;

vec4 godRays(vec4 color) {
    vec2 projectedLightPos = _gr_lightScreenPos.xy * 0.5 + 0.5;

	vec2 deltaTexCoord = texCoord - projectedLightPos;
	deltaTexCoord *= (1.0 / float(_gr_samples)) * _gr_density;

    vec3 rayColor = vec3(0.0);

    float decay = 1.0;
    for (int i = 0; i < _gr_samples; i++) {
		vec2 coord = texCoord - deltaTexCoord * i;

        rayColor += texture(_gr_raysTexture, coord).rgb * decay * _gr_weight;

        decay *= _gr_decayRatio;
	}

    color.rgb += rayColor;

    return color;
}
