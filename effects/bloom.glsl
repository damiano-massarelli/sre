in vec2 texCoord;

uniform sampler2D inputTexture;
uniform sampler2D bloomTexture;
uniform float _bloom_bloomFactor = 0.7;

out vec4 fragColor;

void main() {
    vec4 color = texture(inputTexture, texCoord);
    vec3 blurred = texture(bloomTexture, texCoord).rgb;

    color.rgb += blurred * _bloom_bloomFactor;
    fragColor = color;
}