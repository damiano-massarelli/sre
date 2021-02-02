in vec2 texCoord;

uniform sampler2D inputTexture;
uniform sampler2D aoTexture;
uniform float darkenFactor = 2.2;
uniform int blurSize = 2;

out vec4 fragColor;

void main() {
    vec4 color = texture(inputTexture, texCoord);
    float ao = 0.0;

    vec2 texelSize = 1.0 / vec2(textureSize(aoTexture, 0));
    for (int x = -blurSize; x < blurSize; ++x) {
        for (int y = -blurSize; y < blurSize; ++y) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            ao += texture(aoTexture, texCoord + offset).r;
        }
    }
    ao /= (blurSize * 2 * blurSize * 2);

    float aoFactor = (1.0 - ao);
    aoFactor = pow(aoFactor, darkenFactor);

    fragColor = vec4(color.rgb * aoFactor, 1.0);
}