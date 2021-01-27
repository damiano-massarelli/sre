
const float _MB_GAUSSIAN_WEIGHTS[11] = float[11](0.000003, 0.000229, 0.005977, 0.060598, 0.24173, 0.382925, 0.24173, 0.060598, 0.005977, 0.000229, 0.000003);

in vec2 texCoord;

uniform sampler2D inputTexture;
uniform sampler2D positionTexture;
uniform mat4 _mb_prevProjView;
uniform mat4 _mb_currProjView;
uniform float _mb_blurFactor = 100.0;

out vec4 fragColor;

void main() {
    vec4 color = texture(inputTexture, texCoord);

	vec2 pixSize = 1.0 / textureSize(inputTexture, 0);
    vec3 position = texture(positionTexture, texCoord).xyz;
    vec4 projected = _mb_prevProjView * vec4(position, 1.0);
    vec2 oldPosition = vec2(projected.x, projected.y) / projected.w;
    
    vec4 currProjected = _mb_currProjView * vec4(position, 1.0);
    vec2 currentPosition = vec2(currProjected.x, currProjected.y) / currProjected.w;

    vec2 direction = currentPosition - oldPosition;

    // float(position.z != 0) is a really dirty trick to avoid this effect on fragments for which 
    // position is not available (those fragment there are not rendered in deferred rendering)
    direction = _mb_blurFactor * direction * float(position.z != 0);

    vec3 finalColor = color.rgb * _MB_GAUSSIAN_WEIGHTS[5];

    for (int i = -5; i <= 5; i++) {
        if (i != 0) {
            vec2 sampleCoordinate = clamp(texCoord + direction * pixSize * i, vec2(0.001), vec2(0.999));
            finalColor += texture(inputTexture, sampleCoordinate).rgb * _MB_GAUSSIAN_WEIGHTS[i + 5];
        }
    }
	
    fragColor = vec4(finalColor, color.a);
}
