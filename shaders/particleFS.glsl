layout (location = 0) out vec4 FragColor;

in vec2 texCoord;
in float blend;
in vec4 offsets;

uniform sampler2D atlas;
uniform vec2 frameSize;

void main() {
    vec2 textureCoord = texCoord;

    vec2 offset = offsets.xy;
    vec2 nextOffset = offsets.zw;

    vec2 frameSizeInvY = vec2(frameSize.x, -frameSize.y);
    vec4 current = texture(atlas, offset + texCoord * frameSizeInvY);
    vec4 next = texture(atlas, nextOffset + texCoord * frameSizeInvY);
    FragColor = mix(current, next, blend);
}
