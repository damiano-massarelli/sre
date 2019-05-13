#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
#include <glad/glad.h>

Texture Texture::loadFromFile(const std::string& path, int wrapS, int wrapT)
{
    stbi_set_flip_vertically_on_load(true);

    int width, height, cmp;
    std::uint8_t* data = stbi_load(path.c_str(), &width, &height, &cmp, STBI_rgb_alpha);
    Texture texture;
    if (data == nullptr) {
        std::cerr << "unable to load texture " << path << "\n";
        return texture;
    } else {
        texture = Texture::load(data, width, height, wrapS, wrapT);
        stbi_image_free(data);
    }

    return texture;
}

Texture Texture::loadFromMemory(std::uint8_t* data, std::int32_t len, int wrapS, int wrapT)
{
    stbi_set_flip_vertically_on_load(true);

    int width, height, cmp;
    std::uint8_t* convertedData = stbi_load_from_memory(data, len, &width, &height, &cmp, STBI_rgb_alpha);
    return Texture::load(convertedData, width, height, wrapS, wrapT);
}

Texture Texture::load(std::uint8_t* data, int width, int height, int wrapS, int wrapT)
{
    if (data == nullptr)
        return Texture{0};

    std::uint32_t texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

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
