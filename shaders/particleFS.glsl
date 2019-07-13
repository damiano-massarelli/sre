layout (location = 0) out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D atlas;
uniform vec2 offset;
uniform vec2 nextOffset;
uniform float blend;
uniform vec2 frameSize;

void main() {
    vec2 textureCoord = texCoord;
    vec2 frameSizeInvY = vec2(frameSize.x, -frameSize.y);
    vec4 current = texture(atlas, offset + texCoord * frameSizeInvY);
    vec4 next = texture(atlas, nextOffset + texCoord * frameSizeInvY);
    FragColor = mix(current, next, blend);
}
