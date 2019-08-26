
in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D src;

float radius = 0.1;
uniform vec2 lightScreenPos = vec2(0.5, 0.5);

uniform int samples = 20;

void main() {
	vec2 deltaTexCoord = texCoord - lightScreenPos;
	deltaTexCoord *= 1.0 / float(samples);

	const float radius2 = radius * radius;

	for (int i = 0; i < samples; i++) {
		vec2 coord = texCoord - deltaTexCoord * i;

		float depth = texture(src, coord).r;
		 
	}
}
