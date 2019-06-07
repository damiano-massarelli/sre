
vec4 gammaCorrection(vec4 color) {
    color.rgb = pow(color.rgb, vec3(1/2.2));;

    return color;
}
