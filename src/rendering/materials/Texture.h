#ifndef TEXTURE_H
#define TEXTURE_H
#include <cstdint>
#include <string>

struct Texture {
    /**
      * Loads and returns the texture at a given path.
      *
      * @param path the path of the file to load
      * @return the loaded texture
      */
    static Texture load(const std::string& path);

    /// a value of 0 for this field means invalid
    std::uint32_t id = 0;

    std::string nameInShader;

    /**
      * Checks whether this is a valid (usable texture)
      * @return whether the texture is valid or not (id != 0). */
    bool isValid() const;

    operator bool() const;
};

#endif // TEXTURE_H
