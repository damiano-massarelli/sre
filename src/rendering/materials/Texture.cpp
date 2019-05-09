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
    int numCh;
    std::uint8_t* data = stbi_load(path.c_str(), &width, &height, &numCh, 0);
    if (data == nullptr) {
        std::cerr << "unable to load texture " << path << "\n";
        return Texture{0, ""};
    }

    std::uint32_t texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    GLint format = numCh == 3 ? GL_RGB : GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return Texture{texture, "not_set"};
}

bool Texture::isValid() const
{
    return id != 0;
}

Texture::operator bool() const
{
    return isValid();
}
