uniform samplerCube cubemap;

in vec3 position;

out vec4 FragColor;

void main() {
    vec4 color = texture(cubemap, position);
	color.rgb = pow(color.rgb, vec3(2.2f)); // gamma correction
	FragColor = color;
}
