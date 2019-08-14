
uniform sampler2D bloomTexture;

uniform float _bloom_bloomFactor = 0.7;

vec4 bloom(vec4 color) {
    vec3 blurred = texture(bloomTexture, texCoord).rgb;

    color.rgb += blurred * _bloom_bloomFactor;
    return color;
}