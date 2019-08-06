
uniform float _gc_gamma = 2.2;
uniform float _gc_exposure = 1.0;

vec4 gammaCorrection(vec4 color) {
	// tone mapping
	color.rgb = vec3(1.0) - exp(-color.rgb * _gc_exposure);

	// gamma correction
    color.rgb = pow(color.rgb, vec3(1 / _gc_gamma));;

    return color;
}
