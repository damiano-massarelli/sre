layout (location = 0) out vec4 Diffuse;
layout (location = 1) out vec4 Specular;
layout (location = 2) out vec3 Position;
layout (location = 3) out vec3 Normal;

in vec2 texCoord;
in vec3 position;
in vec3 normal;
in vec4 lightSpacePosition;

uniform sampler2D baseTexture;
uniform sampler2D redTexture;
uniform sampler2D greenTexture;
uniform sampler2D blueTexture;
uniform sampler2D blendTexture;

uniform float horizontalTiles;
uniform float verticalTiles;

void main() {
    vec4 channels = texture2D(blendTexture, texCoord / vec2(horizontalTiles, verticalTiles));
    float baseFactor = 1.0f - (channels.r + channels.g + channels.b);

    vec3 base = vec3(texture2D(baseTexture, texCoord));
    vec3 red = vec3(texture2D(redTexture, texCoord));
    vec3 green = vec3(texture2D(greenTexture, texCoord));
    vec3 blue = vec3(texture2D(blueTexture, texCoord));

    Diffuse.rgb = base * baseFactor + red * channels.r + green * channels.g + blue * channels.b;
	Diffuse.a = 1.0;
	Specular = vec4(0.0);
	Position = position;
	Normal = normal;
}
