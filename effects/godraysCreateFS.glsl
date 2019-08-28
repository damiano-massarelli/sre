
in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D src;

uniform float radius = 0.05;

uniform vec3 lightScreenPos;

uniform vec4 lightColor = vec4(1.0);

uniform vec4 bgColor = vec4(0.0);

void main() {
	vec2 size = textureSize(src, 0);
	vec2 projectedLightPos = lightScreenPos.xy * 0.5 + 0.5;
	float projectedLightZ = lightScreenPos.z * 0.5 + 0.5;

	float depth = texture(src, texCoord).r;

	vec2 dist = texCoord - projectedLightPos;
	dist.x *= size.x / size.y;

	if (length(dist) <= radius) 
		FragColor = lightColor * float(depth > projectedLightZ);
	else
		FragColor = bgColor;
}
