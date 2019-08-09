
const float GAUSSIAN_WEIGHTS[11] = float[11](0.000003, 0.000229, 0.005977, 0.060598, 0.24173, 0.382925, 0.24173, 0.060598, 0.005977, 0.000229, 0.000003);

in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D src;
uniform vec2 direction;

void main() {
	vec2 pixSize = 1.0 / textureSize(src, 0);

	for (int i = -5; i <= 5; i++) {
		vec2 blurTexCoord = texCoord + direction * pixSize * i;
		blurTexCoord = clamp(blurTexCoord, vec2(0.001), vec2(0.999));
		FragColor.rgb += texture(src, blurTexCoord).rgb * GAUSSIAN_WEIGHTS[i + 5];
	}
	
    FragColor.a = 1.0;
}
