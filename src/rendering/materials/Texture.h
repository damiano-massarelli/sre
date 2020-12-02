#ifndef TEXTURE_H
#define TEXTURE_H
#include "resourceManagment/RefCount.h"
#include <cstdint>
#include <glad/glad.h>
#include <map>
#include <string>

/**
 * A drawable image.
 * Textures can be used to display images or as drawing targets */
class Texture {
public:
    RefCount refCount;

    /**
     * Appearance settings used on the loaded texture.
     */
    struct AppearanceSettings {
        bool hasMipmap = true;
        GLint minFilter = GL_LINEAR;
        GLint magFilter = GL_LINEAR;
        GLint wrapS = GL_CLAMP_TO_EDGE;
        GLint wrapT = GL_CLAMP_TO_EDGE;
    };

    /**
     * Settings used when loading a texture.
     * These settings include both data format settings and appearance settings.
     */
    struct Settings {
        GLint internalFormat = GL_RGBA;
        GLenum dataPixelFormat = GL_RGBA;
        GLenum dataPixelType = GL_UNSIGNED_BYTE;
        AppearanceSettings appearanceOptions{};
    };

private:
    static std::map<std::string, Texture> textureCache;

    Settings mSettings;

    std::uint32_t mTextureId = 0;

    Texture(std::uint32_t id);

    int mWidth = 0;
    int mHeight = 0;

    bool mIsCubeMap = false;

    void cleanUpIfNeeded();

    void setupMipmap();

public:
    /**
     * Creates an invalid texture.
     * Use one of the static load* methods to load a texture. */
    Texture() = default;

    /**
     * Loads and returns the texture at a given path.
     * Textures are also cached using their path as the cache key.
     *
     * @param path the path of the file to load
     * @param options appearance settings used on the loaded texture
     * @return the loaded texture
     */
    static Texture loadFromFile(const std::string& path, const AppearanceSettings& options = AppearanceSettings{});

    /**
     * Loads a texture from a buffer in memory.
     * This texture are not cached. If you can
     * specify an identifier to cache textures loaded from
     * memory use loadFromMemoryCached().
     * @param data a pointer to the buffer
     * @param len length of the buffer
     * @param options settigns used when loading the texture.
     * @return the loaded texture */
    static Texture loadFromMemory(
        std::uint8_t* data, std::int32_t len, const AppearanceSettings& options = AppearanceSettings{});

    /**
     * Loads a texture from a buffer in memory and caches it.
     * @sa loadFromMemory()
     * @param cacheKey the name used to cache this texture
     * @param data a pointer to the buffer
     * @param len length of the buffer
     * @param options settings used when loading the texture
     * @return the loaded texture */
    static Texture loadFromMemoryCached(const std::string& cacheKey,
        std::uint8_t* data,
        std::int32_t len,
        const AppearanceSettings& options = AppearanceSettings{});

    /**
     * Creates a new Texture.
     * @param data pixel data of the image
     * @param width width of the texture
     * @param height height of the texture
     * @param options settings used when loading the texture
     */
    static Texture load(void* data, int width, int height, const Settings& options = Settings{});

    /**
     * Loads a cubemap from file.
     * The paths parameter is a map in which the key is the side of the cube
     * (front, bottom, top, back, left, right) and the values are the
     * corresponding paths of the images for those sides.
     * @param paths the paths of the images composing the cubemap.
     * @return a new cubemap */
    static Texture loadCubemapFromFile(const std::map<std::string, std::string>& paths);

    /**
     * Creates a new CubeMap.
     * @param data a map with the correspondence between cube side and the data
     * for that side
     * @param width width of the texture (per single face)
     * @param height height of the texture (per single face)
     * @param wrapS repeat mode on x axis
     * @param wrapT repeat mode on y axis
     * @param wrapR repeat mode on z axis
     * @param mipmap should use mipmap?
     * @param format pixel data format format (GL_RGB, GL_RGBA, etc)
     * @param type the type of data stored in the texture
     * @param internalFormat the format of the data stored in the texture (if
     * GL_REPEAT then it will be the same as format)
     */
    static Texture loadCubemap(const std::map<std::string, void*>& data,
        int width,
        int height,
        int wrapS = GL_REPEAT,
        int wrapT = GL_REPEAT,
        int wrapR = GL_REPEAT,
        int format = GL_RGBA,
        int type = GL_UNSIGNED_BYTE,
        int internalFormat = GL_REPEAT);

    /**
     * Set the mipmap option to the given value.
     * @param isRequred whether mip-maps are required or not
     * @see RenderTarget.
     */
    void setRequireMipmap(bool isRequired);

    /**
     * Regenerates mip maps for this texture.
     * This may be needed if the texture uses mip maps and new content is
     * written on it.
     * @see RenderTarget.
     */
    void updateMipmap() const;

    /**
     * Returns the texture id for this texture.
     * Needed for rendering */
    std::uint32_t getId() const;

    /**
     * @return the width of the texure
     */
    int getWidth() const;

    /**
     * @return the height of the texture
     */
    int getHeight() const;

    /**
     * @return the settings for this texture.
     */
    const Settings& getSettings() const;

    bool isCubeMap() const;

    /**
     * Checks whether this is a valid (usable) texture
     * @return whether the texture is valid or not. */
    bool isValid() const;

    /**
     * Checks whether this is a valid (usable) texture
     * @return whether the texture is valid or not.
     * @sa isValid */
    operator bool() const;

    Texture& operator=(const Texture& rhs);

    virtual ~Texture();
};

using TexturePtr = std::shared_ptr<Texture>;

#endif  // TEXTURE_H
