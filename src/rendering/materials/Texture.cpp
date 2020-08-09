#include "rendering/materials/Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
#include <glad/glad.h>
#include <algorithm>

std::map<std::string, Texture> Texture::textureCache;

Texture Texture::loadFromFile(const std::string& path, const AppearanceSettings& options)
{
	auto cached = textureCache.find(path);
	if (cached != textureCache.end()) 
		return cached->second;
	

    stbi_set_flip_vertically_on_load(true);

    int width, height, cmp;
    std::uint8_t* data = stbi_load(path.c_str(), &width, &height, &cmp, STBI_rgb_alpha);
    Texture texture;
    if (data == nullptr) {
        std::cerr << "unable to load texture " << path << "\n";
        return texture;
    } else {
		const Settings loadOptions{ GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, options };
        texture = Texture::load(data, width, height, loadOptions);
        stbi_image_free(data);
    }

	texture.refCount.onRemove = [path]() { Texture::textureCache.erase(path); };
	textureCache[path] = texture;
	textureCache[path].refCount.setWeak();

    return texture;
}

Texture Texture::loadFromMemory(std::uint8_t* data, std::int32_t len, const AppearanceSettings& options)
{
    stbi_set_flip_vertically_on_load(true);

    int width, height, cmp;
    std::uint8_t* convertedData = stbi_load_from_memory(data, len, &width, &height, &cmp, STBI_rgb_alpha);

	Settings loadOptions{ GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, options };
    return Texture::load(convertedData, width, height, loadOptions);
	if (convertedData) {
		stbi_image_free(convertedData);
	}
}

Texture Texture::loadFromMemoryCached(const std::string& cacheKey, std::uint8_t* data, std::int32_t len, const AppearanceSettings& options)
{
	auto cached = textureCache.find(cacheKey);
	if (cached != textureCache.end()) {
		return cached->second;
	}

	auto texture = loadFromMemory(data, len, options);
	texture.refCount.onRemove = [cacheKey]() { Texture::textureCache.erase(cacheKey); };
	textureCache[cacheKey] = texture;
	textureCache[cacheKey].refCount.setWeak();

	return texture;
}

Texture Texture::loadCubemapFromFile(const std::map<std::string, std::string>& paths)
{
    std::map<std::string, int> side2glSide{
        {"front", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z},
        {"back", GL_TEXTURE_CUBE_MAP_POSITIVE_Z},
        {"top", GL_TEXTURE_CUBE_MAP_POSITIVE_Y},
        {"bottom", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y},
        {"right", GL_TEXTURE_CUBE_MAP_POSITIVE_X},
        {"left", GL_TEXTURE_CUBE_MAP_NEGATIVE_X}
    };

    std::uint32_t cubemap;
    glGenTextures(1, &cubemap);

    stbi_set_flip_vertically_on_load(false);

    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    int width, height, numCh;
    for (auto typePath = paths.begin(); typePath != paths.end(); ++typePath) {
        int side = 0;

        if (side2glSide.count(typePath->first) != 0) {
            side = side2glSide[typePath->first];
        } else {
            std::cout << "Unknown cubemap side " << typePath->first << "\n";
            continue;
        }

        std::uint8_t* data = stbi_load((typePath->second).c_str(), &width, &height, &numCh, STBI_rgb_alpha);
        if (data) {
            glTexImage2D(side, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
            std::cout << "unable to load texture " << typePath->second << "\n";
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	auto t = Texture{ cubemap };
	t.mIsCubeMap = true;
	return t;
}

Texture Texture::loadCubemap(const std::map<std::string, void*>& data, int width, int height,
	int wrapS, int wrapT, int wrapR, int format, int type, int internalFormat)
{
	std::map<std::string, int> side2glSide{
		{"front", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z},
		{"back", GL_TEXTURE_CUBE_MAP_POSITIVE_Z},
		{"top", GL_TEXTURE_CUBE_MAP_POSITIVE_Y},
		{"bottom", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y},
		{"right", GL_TEXTURE_CUBE_MAP_POSITIVE_X},
		{"left", GL_TEXTURE_CUBE_MAP_NEGATIVE_X}
	};

	if (internalFormat == GL_REPEAT) internalFormat = format;

	std::uint32_t cubemap;
	glGenTextures(1, &cubemap);

	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	for (auto& [face, data] : data) {
		int side = 0;

		if (side2glSide.count(face) != 0) {
			side = side2glSide[face];
		}
		else {
			std::cout << "Unknown cubemap side " << face << "\n";
			continue;
		}
		
		glTexImage2D(side, 0, internalFormat, width, height, 0, format, type, data);

	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrapR);

	auto t = Texture{ cubemap };
	t.mIsCubeMap = true;
	return t;
}

Texture Texture::load(void* data, int width, int height, const Settings& options)
{
    std::uint32_t texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, options.internalFormat, width, height, 0, options.dataPixelFormat, options.dataPixelType, data);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, options.appearanceOptions.minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, options.appearanceOptions.magFilter);

	if (options.appearanceOptions.createMipmap) {
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		if (GLAD_GL_ARB_texture_filter_anisotropic) {
			float maxAniso = 0;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, std::min(maxAniso, 4.0f));
		}
		else {
			std::cout << "anisotropic filtering not available\n";
		}
	}
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, options.appearanceOptions.wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, options.appearanceOptions.wrapT);

    glBindTexture(GL_TEXTURE_2D, 0);

    auto tex = Texture{texture};
	tex.mWidth = width;
	tex.mHeight = height;
	tex.mSettings = options;

	return tex;
}

Texture::Texture(std::uint32_t id) : mTextureId{id}
{}

void Texture::cleanUpIfNeeded()
{
	if (refCount.shouldCleanUp() && mTextureId != 0) {
		glDeleteTextures(1, &mTextureId);
		mTextureId = 0;
	}
}

void Texture::regenerateMipmap() const
{
	if (mSettings.appearanceOptions.createMipmap && isValid()) {
		glBindTexture(GL_TEXTURE_2D, mTextureId);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

uint32_t Texture::getId() const
{
    return mTextureId;
}

int Texture::getWidth() const
{
	return mWidth;
}

int Texture::getHeight() const
{
	return mHeight;
}

const Texture::Settings& Texture::getSettings() const
{
	return mSettings;
}

bool Texture::isCubeMap() const
{
	return mIsCubeMap;
}

bool Texture::isValid() const
{
    return mTextureId != 0;
}

Texture::operator bool() const
{
    return isValid();
}

Texture& Texture::operator=(const Texture& rhs)
{
	if (mTextureId == rhs.mTextureId) return *this;

	cleanUpIfNeeded();

	mTextureId = rhs.mTextureId;
	mWidth = rhs.mWidth;
	mHeight = rhs.mHeight;
	mIsCubeMap = rhs.mIsCubeMap;
	mSettings = rhs.mSettings;

	refCount = rhs.refCount;

	return *this;
}

Texture::~Texture()
{
	cleanUpIfNeeded();
}
