uniform samplerCube cubemap;

in vec3 position;

out vec4 FragColor;

void main() {
    FragColor = texture(cubemap, position);
}
