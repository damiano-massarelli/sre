in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D src;
uniform int channel;

void main() {
    vec4 color = texture(src, texCoord);

    FragColor = vec4(color[channel], 0.0, 0.0, 1.0);
}
