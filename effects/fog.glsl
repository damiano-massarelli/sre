uniform vec3 uFogColor;
uniform vec2 uFogParams;

vec4 fog(vec4 color) {
    float z = texture(depthTexture, texCoord).r;

    // Linear depth
    z = 2.0 * z - 1.0;
    float dist = 2.0 * cameraNear * cameraFar / (cameraFar + cameraNear - z * (cameraFar - cameraNear));

    // 0 means not in fog, 1 means in fog
    float fogFactor = 1 - exp(-pow(dist * uFogParams.x, uFogParams.y));
    
    color.xyz = mix(color.xyz, uFogColor, fogFactor);

    return color;
}
