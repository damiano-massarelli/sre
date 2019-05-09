/** Lights should be imported
  * All positions and directions are expected to be in world space
  */
vec3 phongComputeColor(Light light, vec3 diffuseColor, vec3 specularColor, float shininess, vec3 fragPosition, vec3 fragNormal, vec3 cameraPosition) {
    vec3 outcolor = vec3(0.0f);

    // ambient component
    outcolor += diffuseColor * light.ambientColor;

    // diffuse component
    vec3 rayToLight;
    float lightDist = 0.0f;
    if (light.type == LIGHT_TYPE_DIRECTIONAL) {
        /* The direction of a directional light is constant
         * Since rayToLight goes from the fragment to the light
         * light.direction must be inverted */
        rayToLight = -light.direction;
    } else {
        rayToLight = light.position - fragPosition;
        lightDist = length(rayToLight);
    }
    rayToLight = normalize(rayToLight);
    float diffuseIntensity = max(dot(fragNormal, rayToLight), 0.0f);
    outcolor += light.diffuseColor * diffuseColor * diffuseIntensity;

    // specular component
    vec3 rayToCamera = normalize(cameraPosition - fragPosition);
    vec3 reflectedLight = reflect(-rayToLight, fragNormal);
    float specularIntensity = pow(max(dot(reflectedLight, rayToCamera), 0.0f), shininess);
    outcolor += light.specularColor * specularColor * specularIntensity;

    float attenuation = 1.0f;
    if (light.type == LIGHT_TYPE_POINT || light.type == LIGHT_TYPE_SPOT)
        attenuation = 1 / (light.attenuations.x + lightDist * light.attenuations.y + lightDist * lightDist * light.attenuations.z);

    if (light.type == LIGHT_TYPE_SPOT) {
        float theta = dot(-rayToLight, light.direction);
        attenuation *= clamp((theta - light.spotAngles.y) / (light.spotAngles.x - light.spotAngles.y), 0.0f, 1.0f);
    }

    // be careful when alpha is used
    return outcolor * attenuation;
}
