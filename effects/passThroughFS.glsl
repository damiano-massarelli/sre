in vec2 texCoord;

uniform sampler2D src;
out vec4 fragColor;

void main() {
    fragColor = texture(src, texCoord);
}