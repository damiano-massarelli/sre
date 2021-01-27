in vec2 texCoord;

uniform sampler2D inputTexture;
uniform sampler2D reflectionTexture;
const int _SSR_MAX_LOD = 6;

out vec4 fragColor;

void main() {
	vec4 color = texture(inputTexture, texCoord);
	float roughness = texture(reflectionTexture, texCoord).a;
	vec3 reflectionColor = textureLod(reflectionTexture, texCoord, roughness * _SSR_MAX_LOD).rgb;

	vec3 resultColor = reflectionColor * (1. - roughness);
	fragColor = color + vec4(resultColor, 0.);
}