
uniform float _gc_gamma = 2.2;
uniform float _gc_exposure = 1.0;

vec3 _gc_toGammaCorrectedLDR(vec3 hdrLinearColor) {
	// toneMapping
	const vec3 ldrColor = vec3(1.0) - exp(-hdrLinearColor * _gc_exposure);

	// gamma correction
	const vec3 gammaCorrected = pow(ldrColor, vec3(1 / _gc_gamma));

	return gammaCorrected;
}

vec4 gammaCorrection(vec4 color) {
	// tone mapping
	color.rgb = _gc_toGammaCorrectedLDR(color.rgb);

    return color;
}
