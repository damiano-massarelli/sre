#ifndef TEXTURE_H
#define TEXTURE_H
#include <cstdint>
#include <string>
#include <glad/glad.h>

/**
 * A drawable image.
 * Textures can be used to display images or as drawing targets */
class Texture {
    private:
        std::uint32_t mTextureId = 0;

        Texture(std::uint32_t id);

        static Texture load(std::uint8_t* data, int width, int height, int wrapS, int wrapT);

    public:
        /**
          * Creates an invalid texture.
          * Use one of the static load* methods to load a texture. */
        Texture() = default;

        /**
          * Loads and returns the texture at a given path.
          *
          * @param path the path of the file to load
          * @return the loaded texture
          */
        static Texture loadFromFile(const std::string& path, int wrapS = GL_REPEAT, int wrapT = GL_REPEAT);

        /**
          * Loads a texture from a buffer in memory.
          * @param data a pointer to the buffer
          * @param length of the buffer
          * @return the loaded texture */
        static Texture loadFromMemory(std::uint8_t* data, std::int32_t len, int wrapS = GL_REPEAT, int wrapT = GL_REPEAT);

        std::string nameInShader;

        /**
          * Returns the texture id for this texture.
          * Needed for rendering */
        std::uint32_t getId();

        /**
          * Checks whether this is a valid (usable) texture
          * @return whether the texture is valid or not. */
        bool isValid() const;

        /**
          * Checks whether this is a valid (usable) texture
          * @return whether the texture is valid or not.
          * @sa isValid */
        operator bool() const;
};

#endif // TEXTURE_H
