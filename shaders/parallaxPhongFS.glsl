const float PARALLAX_HEIGHT = 0.115;
const float MIN_LAYERS = 8.0f;
const float MAX_LAYERS = 32.0f;

// when distance is less than this value parallax mapping is disabled
const float PARALLAX_MIN_DISTANCE = 13.5;

// when distance is greater than this value parallax mapping is fully operative
const float PARALLAX_FULL_DISTANCE = 18.5;

struct PhongMaterial {
    bool useDiffuseMap;
    sampler2D diffuse;
    vec3 diffuseColor;

    bool useSpecularMap;
    sampler2D specular;
    vec3 specularColor;

    sampler2D bump;
    sampler2D parallax;

    float shininess;

    float opacity;
};

in vec2 texCoord;
in vec3 position;
in vec4 lightSpacePosition;

// bump mapping specific ins
in mat3 tangentToWorldSpace;
in vec3 tangentSpaceRayToCamera;

out vec4 FragColor;

uniform PhongMaterial material;

layout (std140) uniform Lights {
    int numLights;
    Light lights[10];
};

layout (std140) uniform Camera {
    vec3 cameraPosition;
    vec3 cameraDirection;
};

vec2 parallaxMapping(vec2 textureCoord, vec3 rayToCamera) {
    float height = texture(material.parallax, textureCoord).r;

    float layers = mix(MAX_LAYERS, MIN_LAYERS, max(rayToCamera.z, 0.0));

    float currentLayerDepth = 1.0;
    float layerDepthDecrease = 1.0 / layers;

    float currentMapDepth = texture(material.parallax, textureCoord).r;
    vec2 currentDepthTexCoord = textureCoord;
    vec2 textureIncrease = (rayToCamera.xy * PARALLAX_HEIGHT) / layers;

    while (currentLayerDepth > currentMapDepth) {
        currentLayerDepth -= layerDepthDecrease;
        currentDepthTexCoord += textureIncrease;

        currentMapDepth = texture(material.parallax, textureCoord).r;
    }

    return currentDepthTexCoord;
}

void main() {
    float distanceToCamera = distance(cameraPosition, position);

    // apply parallax mapping
    vec2 parallaxTexCoord = parallaxMapping(texCoord, tangentSpaceRayToCamera);
    parallaxTexCoord = mix(texCoord, parallaxTexCoord, clamp((distanceToCamera - PARALLAX_MIN_DISTANCE) / (PARALLAX_FULL_DISTANCE - PARALLAX_MIN_DISTANCE), 0.0, 1.0));
    if (parallaxTexCoord.x > 1.0 || parallaxTexCoord.y > 1.0 || parallaxTexCoord.x < 0.0 || parallaxTexCoord.y < 0.0)
        discard;

    // get normal from bump map
    vec3 normal = (texture(material.bump, parallaxTexCoord).rgb) * 2.0 - 1.0;
    // from tangent space to world space
    normal = normalize(tangentToWorldSpace * normal);

	vec3 shadowSampleCoord = lightSpacePosition.xyz / lightSpacePosition.w;
	shadowSampleCoord = (shadowSampleCoord + vec3(1.0)) / 2.0;

	// TODO check the value of w to see if
	// perspective projection is being used. If that's the case linearize depth.
	float depthInShadowMap = texture(shadowMap, shadowSampleCoord.xy).r;

	vec4 sampledDiffuseColor = texture(material.diffuse, parallaxTexCoord);
	if (material.opacity == 0.0f || sampledDiffuseColor.a < 0.01) discard;

    vec3 diffuseColor = material.diffuseColor;
    if (material.useDiffuseMap)
        diffuseColor *= sampledDiffuseColor.rgb;

    vec3 specularColor = material.specularColor;
    if (material.useSpecularMap)
        specularColor *= vec3(texture(material.specular, parallaxTexCoord));

	//diffuseColor = pow(diffuseColor, vec3(2.2));
	//specularColor = pow(specularColor, vec3(2.2));
    vec3 color = vec3(0.0f);
    for (int i = 0; i < numLights; i++) {
        color += phongComputeColor(lights[0], diffuseColor, specularColor, material.shininess, position, normal, cameraPosition, lightSpacePosition, i == 0);
    }

	// apply fog
	color = fogger(color, distanceToCamera);

    FragColor = vec4(color, material.opacity);
}
