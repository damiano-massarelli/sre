
uniform sampler2D bloomTexture;

vec4 bloom(vec4 color) {
    vec3 blurred = texture(bloomTexture, texCoord).rgb;

    color.rgb += blurred;
    return color;
}