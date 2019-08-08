uniform sampler2D _ssao_texture;

vec4 ssao(vec4 color) {
    float ao = texture(_ssao_texture, texCoord).r;

    return vec4(ao, ao, ao, 1.0);
}