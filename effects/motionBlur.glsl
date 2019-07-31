
const float _MB_GAUSSIAN_WEIGHTS[11] = float[11](0.000003, 0.000229, 0.005977, 0.060598, 0.24173, 0.382925, 0.24173, 0.060598, 0.005977, 0.000229, 0.000003);

uniform sampler2D _mbPositionTexture;
uniform mat4 _mbPrevProjView;
uniform float _mbBlurFactor = 100.0;

vec4 motionBlur(vec4 color) {
	vec2 pixSize = 1.0 / textureSize(screenTexture, 0);
    vec3 position = texture(_mbPositionTexture, texCoord).xyz;
    vec4 projected = _mbPrevProjView * vec4(position, 1.0);
    vec2 oldPosition = vec2(projected.x, projected.y) / projected.w;
    oldPosition = oldPosition / 2 + 0.5;

    vec2 direction = texCoord - oldPosition;

    // float(position.z != 0) is a really dirty trick to avoid this effect on fragments for which 
    // position is not available (those fragment there are not rendered in deferred rendering)
    direction = _mbBlurFactor * direction * float(position.z != 0);

    vec3 finalColor = color.rgb * _MB_GAUSSIAN_WEIGHTS[5];

    for (int i = -5; i <= 5; i++) {
        if (i != 0) {
            vec2 sampleCoordinate = clamp(texCoord + direction * pixSize * i, vec2(0.0001), vec2(0.9999));
            finalColor += texture(screenTexture, sampleCoordinate).rgb * _MB_GAUSSIAN_WEIGHTS[i + 5];
        }
    }
	
    return vec4(finalColor, color.a);
}
