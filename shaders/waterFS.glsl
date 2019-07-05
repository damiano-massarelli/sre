out vec4 FragColor;
in vec2 texCoord;


uniform sampler2D reflection;

void main() {
    FragColor = vec4(texture(reflection, texCoord).rgb, 1);
}