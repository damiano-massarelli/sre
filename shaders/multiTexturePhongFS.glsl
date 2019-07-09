layout (location = 0) out vec4 Diffuse;
layout (location = 1) out vec4 Specular;
layout (location = 2) out vec3 Position;
layout (location = 3) out vec3 Normal;

uniform sampler2D baseTexture;
uniform sampler2D baseTextureBump;
uniform sampler2D redTexture;
uniform sampler2D redTextureSpecular;
uniform sampler2D redTextureBump;
uniform sampler2D greenTexture;
uniform sampler2D greenTextureSpecular;
uniform sampler2D greenTextureBump;
uniform sampler2D blendTexture;

uniform float redShininess;
uniform float greenShininess;

in vec2 texCoord;
in vec3 position;

// bump mapping specific ins
in mat3 tangentToWorldSpace;

void main() {
    float verticalTiles = 40.0;
    float horizontalTiles = 40.0;
    vec4 channels = texture2D(blendTexture, texCoord / vec2(horizontalTiles, verticalTiles));
    float baseFactor = 1.0f - (channels.r + channels.g);

    vec3 base = vec3(texture2D(baseTexture, texCoord));
    vec3 red = vec3(texture2D(redTexture, texCoord));
    vec3 green = vec3(texture2D(greenTexture, texCoord));

    vec3 diffuseColor = base * baseFactor + red * channels.r + green * channels.g;

    vec3 redSpecular = vec3(texture2D(redTextureSpecular, texCoord));
    vec3 greenSpecular = vec3(texture2D(greenTextureSpecular, texCoord));

    vec3 specularColor = vec3(0.0) * baseFactor + redSpecular * channels.r + greenSpecular * channels.g;

    // get normal from bump map
    vec3 baseNormal = (texture(baseTextureBump, texCoord).rgb) * 2.0 - 1.0;
    vec3 redNormal = (texture(redTextureBump, texCoord).rgb) * 2.0 - 1.0;
    vec3 greenNormal = (texture(greenTextureBump, texCoord).rgb) * 2.0 - 1.0;
    vec3 normal = baseNormal * baseFactor + redNormal * channels.r + greenNormal * channels.g;
    normal = normalize(tangentToWorldSpace * normal);

    float shininess = redShininess * channels.r + greenShininess * channels.g;

	Position = position;
	Normal = normal;
	Diffuse = vec4(diffuseColor, 1.0);
	Specular = vec4(specularColor, shininess);
}
