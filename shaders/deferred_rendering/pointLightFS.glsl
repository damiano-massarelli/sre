layout (location = 0) out vec4 FragColor;

layout (std140) uniform Lights {
    int numLights;
    Light lights[10];
};

layout (std140) uniform Camera {
    vec3 cameraPosition;
    vec3 cameraDirection;
};

vec3 phongComputeColor(Light light, vec3 diffuseColor, vec3 specularColor, float shininess, vec3 fragPosition, vec3 fragNormal, vec3 cameraPosition) {
    vec3 outcolor = vec3(0.0f);

    // ambient component
    outcolor += diffuseColor * light.ambientColor;

    // diffuse component
    vec3 rayToLight;
    float lightDist = 0.0f;
 
    rayToLight = light.position - fragPosition;
    lightDist = length(rayToLight);
    rayToLight = normalize(rayToLight);

    float inShadow = 0.0;
    if (light.castShadow)
        inShadow = pointMapIsInShadow(fragPosition, light.position);

    float diffuseIntensity = max(dot(fragNormal, rayToLight), 0.0f);
    outcolor += (light.diffuseColor * diffuseColor * diffuseIntensity) * (1.0 - inShadow);

    if (shininess != 0) { // Phong or Lambert material? if shininess == 0 it is Lambert
        // specular component
        vec3 rayToCamera = normalize(cameraPosition - fragPosition);
        vec3 halfWay = normalize(rayToCamera + rayToLight);

        float specularIntensity = pow(max(dot(halfWay, fragNormal), 0.0f), shininess);
        outcolor += (light.specularColor * specularColor * specularIntensity * float(diffuseIntensity > 0)) * (1.0 - inShadow);
    }

    float attenuation = 1.0f;
    attenuation = 1.0 / (light.attenuations.x + lightDist * light.attenuations.y + lightDist * lightDist * light.attenuations.z);

    // be careful when alpha is used
    return outcolor * attenuation;
}

uniform sampler2D DiffuseData;
uniform sampler2D SpecularData;
uniform sampler2D PositionData;
uniform sampler2D NormalData;

uniform int lightIndex;

void main() {
    vec2 texCoord = gl_FragCoord.xy / textureSize(DiffuseData, 0);

    vec3 diffuseColor = texture(DiffuseData, texCoord).rgb;
    vec4 specularSample = texture(SpecularData, texCoord);
    vec3 specularColor = specularSample.rgb;
    float shininess = specularSample.a;
    vec3 normal = texture(NormalData, texCoord).xyz;
    vec3 position = texture(PositionData, texCoord).xyz;
    vec3 color = phongComputeColor(lights[lightIndex], diffuseColor, specularColor, shininess, position, normal, cameraPosition);

    FragColor = vec4(color, 1.0);
}
