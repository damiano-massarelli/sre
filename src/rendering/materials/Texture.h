#ifndef TEXTURE_H
#define TEXTURE_H
#include <cstdint>
#include <string>

class Texture {
    private:
        std::uint32_t mTextureId = 0;

        Texture(std::uint32_t id);

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
        static Texture load(const std::string& path);

        std::string nameInShader;

        /**
          * Returns the texture id for this texture.
          * Needed for rendering */
        std::uint32_t getId();

        /**
          * Checks whether this is a valid (usable) texture
          * @return whether the texture is valid or not.
          * @sa operator bool */
        bool isValid() const;

        /**
          * Checks whether this is a valid (usable) texture
          * @return whether the texture is valid or not.
          * @sa isValid */
        operator bool() const;
};

#endif // TEXTURE_H
