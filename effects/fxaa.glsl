const vec3 LUMA = vec3(0.299, 0.587, 0.114);
const vec2 FXAA_SPAN = vec2(8.0f, 8.0f);
const float FXAA_REDUCE_MUL = 1 / 8.0;
const float FXAA_REDUCE_MIN = 1 / 128.0;

/** if 1 textures are preserved and not blurred by fxaa but anti-aliasing might not be as
  * effective as it should. If -> 0, anti-aliasing is super effective but textures might be
  * too blurred */
const float FXAA_PRESERVE_TEXTURES = 1.0 / 10.5;

uniform vec2 pixelSize;
uniform float near;
uniform float far;

float fxaaDepthAt(vec2 coord) {
    float z = texture2D(depthTexture, coord).r;
    float d = (2.0 * near * far) / (far + near - z * (far - near));

    return d / far;
}

float fxaaDepthDiff(vec2 coord, vec2 coord2) {
    float diff = abs(fxaaDepthAt(coord) - fxaaDepthAt(coord2));
    return pow(diff, FXAA_PRESERVE_TEXTURES);
}

vec4 fxaa(vec4 color) {
    float lumaTL = dot( texture2D(screenTexture, texCoord + vec2(-1.0, 1.0) * pixelSize ).rgb, LUMA );
    float lumaTR = dot( texture2D(screenTexture, texCoord + vec2(1.0, 1.0) * pixelSize ).rgb, LUMA );
    float lumaBL = dot( texture2D(screenTexture, texCoord + vec2(-1.0, -1.0) * pixelSize ).rgb, LUMA );
    float lumaBR = dot( texture2D(screenTexture, texCoord + vec2(1.0, -1.0) * pixelSize ).rgb, LUMA );

    vec2 dir;
    dir.x = -((lumaTL + lumaTR) - (lumaBL + lumaBR));
    dir.y = ((lumaTL + lumaBL) - (lumaTR + lumaBR));

    float dirReduce = max(FXAA_REDUCE_MUL * (lumaTL + lumaTR + lumaBL + lumaBR) / 4.0, FXAA_REDUCE_MIN);
    dir /= (min(abs(dir.x), abs(dir.y)) + dirReduce);

    dir = clamp(dir, -FXAA_SPAN, FXAA_SPAN);

    vec2 texelSizeDir = dir * pixelSize;

    // sample back and forth along the direction
    vec3 sample1 = 0.5 * (
        texture2D(screenTexture, texCoord + texelSizeDir * vec2(1/3.0 - 0.5)).rgb +
        texture2D(screenTexture, texCoord + texelSizeDir * vec2(2/3.0 - 0.5)).rgb
    );

    // just like before but a little further away
    vec3 sample2 = 0.5 * sample1 + 0.25 * (
        texture2D(screenTexture, texCoord + texelSizeDir * vec2(0 - 0.5)).rgb +
        texture2D(screenTexture, texCoord + texelSizeDir * vec2(1.0 - 0.5)).rgb
    );

    float diff21 = fxaaDepthDiff(texCoord, texCoord + texelSizeDir * vec2(-0.5));
    float diff22 = fxaaDepthDiff(texCoord, texCoord + texelSizeDir * vec2(0.5));
    float diff2 = (diff21 + diff22) / 2;

	if (texCoord.s < 0.499)
		color = color;
	else if (texCoord.s >= 0.499 && texCoord.s <= 0.501)
		color = vec4(0);
	else
		color = vec4(mix(color.rgb, sample2, diff2), color.a);

    return color;
}
