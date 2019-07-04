
out vec4 FragColor;
in vec2 texCoord;


uniform sampler2D reflection;

void main() {
    FragColor = texture(reflection, texCoord);
}