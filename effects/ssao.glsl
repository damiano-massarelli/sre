uniform sampler2D _ssao_texture;

vec4 ssao(vec4 color) {
    float ao = 0.0;

    vec2 texelSize = 1.0 / vec2(textureSize(_ssao_texture, 0));
    for (int x = -2; x < 2; ++x) {
        for (int y = -2; y < 2; ++y) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            ao += texture(_ssao_texture, texCoord + offset).r;
        }
    }
    ao /= (4.0 * 4.0);

    float aoFactor = (1.0 - ao);
    aoFactor = pow(aoFactor, 3.0);

    return vec4(color.rgb * aoFactor, 1.0);
}