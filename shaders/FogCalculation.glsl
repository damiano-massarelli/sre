layout (std140) uniform Fog {
    vec3 _fogColor;
    vec2 _fogParams; // x: inverse distance, y: rapidity
};

vec3 fogger(vec3 color, float dist) {
    float fogFactor = exp(-pow(dist * _fogParams.x, _fogParams.y));
    color = mix(color, _fogColor, 1.0 - fogFactor);
    return color;
}
