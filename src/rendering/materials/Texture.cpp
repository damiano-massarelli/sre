#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
#include <glad/glad.h>

Texture Texture::load(const std::string& path)
{
    stbi_set_flip_vertically_on_load(true);

    int width;
    int height;
    std::uint8_t* data = stbi_load(path.c_str(), &width, &height, nullptr, STBI_rgb_alpha);
    if (data == nullptr) {
        std::cerr << "unable to load texture " << path << "\n";
        return Texture{0};
    }

    std::uint32_t texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return Texture{texture};
}

Texture::Texture(std::uint32_t id) : mTextureId{id}
{

}

uint32_t Texture::getId()
{
    return mTextureId;
}

bool Texture::isValid() const
{
    return mTextureId != 0;
}

Texture::operator bool() const
{
    return isValid();
}
