uniform sampler2D _ssao_texture;

uniform float _ssao_darkenFactor = 2.2;
uniform int _ssao_blurSize = 2;

vec4 ssao(vec4 color) {
    float ao = 0.0;

    vec2 texelSize = 1.0 / vec2(textureSize(_ssao_texture, 0));
    for (int x = -_ssao_blurSize; x < _ssao_blurSize; ++x) {
        for (int y = -_ssao_blurSize; y < _ssao_blurSize; ++y) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            ao += texture(_ssao_texture, texCoord + offset).r;
        }
    }
    ao /= (_ssao_blurSize * 2 * _ssao_blurSize * 2);

    float aoFactor = (1.0 - ao);
    aoFactor = pow(aoFactor, _ssao_darkenFactor);

    return vec4(color.rgb * aoFactor, 1.0);
}