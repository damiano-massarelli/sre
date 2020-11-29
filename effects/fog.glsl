uniform vec3 uFogColor;
uniform vec2 uFogParams;

vec4 fog(vec4 color) {
    float dist = texture(depthTexture, texCoord).r;
    float fogFactor = exp(-pow(dist * uFogParams.x, uFogParams.y));
    
    color.xyz = mix(color.xyz, uFogColor, 1 - fogFactor);

    return color;
}
