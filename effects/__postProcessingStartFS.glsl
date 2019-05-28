in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D screenTexture;
uniform sampler2D depthTexture;

void main() {
	vec3 color = texture(screenTexture, texCoord);
