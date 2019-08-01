const vec3 BLOOM_LUMA = vec3(0.299, 0.587, 0.114);

in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D src;

void main() {
    vec4 color = texture2D(src, texCoord);
    float luma = dot(color.rgb, BLOOM_LUMA);

    FragColor.rgb = color.rgb * luma * luma;
    FragColor.a = color.a;
}
