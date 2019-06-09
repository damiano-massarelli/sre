//uniform vec2 pixelSize;

float near = 0.1;
float far = 200.0;

float fxaaDepthAt(vec2 coord) {
    float z = texture2D(depthTexture, coord).r * 2.0 - 1.0;
    float d = (2.0 * near * far) / (far + near - z * (far - near));

    return d / far;
}

float fxaaDepthDiff(vec2 coord, vec2 coord2) {
    float diff = abs(fxaaDepthAt(coord) - fxaaDepthAt(coord2));
    return pow(diff, 1.0 / 2.2);
}

const vec3 LUMA = vec3(0.299, 0.587, 0.114);
const vec2 FXAA_SPAN = vec2(16.0f, 16.0f);
const float FXAA_REDUCE_MUL = 1 / 16.0;
const float FXAA_REDUCE_MIN = 1 / 128.0;

vec4 fxaa(vec4 color) {
    vec2 pixelSize = vec2(0.00078125, 0.00078125);
    float lumaTL = dot( texture2D(screenTexture, texCoord + vec2(-1.0, 1.0) * pixelSize ).rgb, LUMA );
    float lumaTR = dot( texture2D(screenTexture, texCoord + vec2(1.0, 1.0) * pixelSize ).rgb, LUMA );
    float lumaBL = dot( texture2D(screenTexture, texCoord + vec2(-1.0, -1.0) * pixelSize ).rgb, LUMA );
    float lumaBR = dot( texture2D(screenTexture, texCoord + vec2(1.0, -1.0) * pixelSize ).rgb, LUMA );
    float lumaM  = dot(color.rgb, LUMA);

    vec2 dir;
    dir.x = -((lumaTL + lumaTR) - (lumaBL + lumaBR));
    dir.y = -((lumaTL + lumaBL) - (lumaTR + lumaBR));

    float dirReduce = max(FXAA_REDUCE_MUL * (lumaTL + lumaTR + lumaBL + lumaBR) / 4.0, FXAA_REDUCE_MIN);
    dir /= (min(abs(dir.x), abs(dir.y)) + dirReduce);

    dir = clamp(dir, -FXAA_SPAN, FXAA_SPAN);

    vec2 texelSizeDir = dir * pixelSize;

    // sample back and forth along the direction
    vec3 sample1 = 0.3 * (
        texture2D(screenTexture, texCoord + texelSizeDir * vec2(1/3.0 - 0.5)).rgb +
        texture2D(screenTexture, texCoord + texelSizeDir * vec2(2/3.0 - 0.5)).rgb
    );
    float diff11 = fxaaDepthDiff(texCoord, texCoord + texelSizeDir * vec2(1/3.0 - 0.5));
    float diff12 = fxaaDepthDiff(texCoord, texCoord + texelSizeDir * vec2(2/3.0 - 0.5));
    float diff1 = (diff11 + diff12) / 2;

    // just like before but a little further away
    vec3 sample2 = 0.5 * sample1 + 0.25 * (
        texture2D(screenTexture, texCoord + texelSizeDir * vec2(0 - 0.5)).rgb +
        texture2D(screenTexture, texCoord + texelSizeDir * vec2(1.0 - 0.5)).rgb
    );

    float diff21 = fxaaDepthDiff(texCoord, texCoord + texelSizeDir * vec2(0 - 0.5));
    float diff22 = fxaaDepthDiff(texCoord, texCoord + texelSizeDir * vec2(1 - 0.5));
    float diff2 = (diff21 + diff22) / 2;

    color = vec4(mix(color.rgb, sample2, diff2), color.a);

    return color;
}
