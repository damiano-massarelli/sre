#include "GameObjectLoader.h"
#include "Engine.h"
#include "MeshLoader.h"
#include "PhongMaterial.h"
#include "Transform.h"
#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <glm/gtx/matrix_decompose.hpp>

/** Transpose the assimp aiMatrix4x4 which is a row major matrix
  * @param a row major assimp matrix
  * @return a column major glm matrix */
glm::mat4 convertMatrix(const aiMatrix4x4 &aiMat)
{
    return {
        aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1,
        aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2,
        aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3,
        aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4
    };
}

GameObjectEH GameObjectLoader::processNode(aiNode* node, const aiScene* scene)
{
    GameObjectEH go = Engine::gameObjectManager.createGameObject();
    go->name = std::string{node->mName.C_Str()};

    std::cout << "at node " << node->mName.C_Str() << "\n";
    for (std::uint32_t i = 0; i < node->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        std::cout << "loading mesh " << mesh->mName.C_Str() << "\n";
        processMesh(go, mesh, scene);
    }

    for (std::uint32_t i = 0; i < node->mNumChildren; ++i) {
		/* This must be a two steps process:
		 * When go->transform is evaluated it returns a pointer to an element of a vector,
		 * when a new child is added this pointer may become invalid.
		 * Hence, using go->transform.addChild(processNode(node->mChildren[i], scene)); causes
		 * undefined behavior since go->transform is evaluated before a new game object is added */
		auto child = processNode(node->mChildren[i], scene);
        go->transform.addChild(child);
    }

    /* assimp only provides the transformation matrix relative to the parent node
     * hence we need to extract position, rotation and scale to obtain the local
     * components of that matrix */
    glm::vec3 position, scale, skew;
    glm::quat rotation;
    glm::vec4 perspective;
    glm::decompose(convertMatrix(node->mTransformation) , scale, rotation, position, skew, perspective);

    /* There is no need to use local transformations since when the following code is executed
     * all the children have their position already set relative to the parent (thanks to the
     * decomposed matrix). When the following transformations are applied they also affect the
     * children of this node */
    go->transform.setPosition(position);
    go->transform.setRotation(rotation);
    go->transform.setScale(scale);
    return go;
}

void GameObjectLoader::processMesh(const GameObjectEH& go, aiMesh* mesh, const aiScene* scene)
{
    // data regarding vertices: positions, normals, texture coords
    std::vector<float> vertexData;

    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> texCoords;

    std::vector<Vertex> vertices;
    std::vector<std::uint32_t> indices;

    // Load vertex data
    for (std::uint32_t i = 0; i < mesh->mNumVertices; ++i) {
        Vertex v;

        // position
        vertexData.insert(vertexData.end(), {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z});
        positions.insert(positions.end(), {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z});
        v.position = glm::vec3{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};

        // normal, if not present 0, 0, 0 is used
        if (mesh->HasNormals()) {
            vertexData.insert(vertexData.end(), {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z});
            normals.insert(normals.end(), {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z});
            v.normal = glm::vec3{mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
        } else {
            std::cout << "cannot find normals, setting them to (0, 0, 0)\n";
            vertexData.insert(vertexData.end(), {0.0f, 0.0f, 0.0f});
            normals.insert(normals.end(), {0.0f, 0.0f, 0.0f});
            v.normal = glm::vec3{0.0f, 0.0f, 0.0f};
        }

        // if texture coordinates are not available just put (0, 0)
        if (mesh->mTextureCoords[0]) {
            vertexData.insert(vertexData.end(), {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y});
            texCoords.insert(texCoords.end(), {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y});
            v.texCoord = glm::vec2{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        } else {
            std::cout << "cannot find uv coords, setting them to (0, 0)\n";
            vertexData.insert(vertexData.end(), {0.0f, 0.0f});
            texCoords.insert(texCoords.end(), {0.0f, 0.0f});
            v.texCoord = glm::vec2{0.0f, 0.0f};
        }

        vertices.push_back(v);
    }

    // Load indices
    for (std::uint32_t i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];
        for (std::uint32_t j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    MeshLoader loader;
    loader.loadData(positions.data(), positions.size(), 3);
    loader.loadData(normals.data(), normals.size(), 3);
    loader.loadData(texCoords.data(), texCoords.size(), 2);
    loader.loadData(indices.data(), indices.size(), 0, GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_INT);

    Mesh loadedMesh = loader.getMesh(vertices.size(), indices.size());

    MaterialPtr loadedMaterial = processMaterial(mesh, scene);
    if (loadedMaterial == nullptr) {
        std::cerr << "Mesh " << mesh->mName.C_Str() << "does not have a corresponding material, discarded\n";
        return;
    }

    go->addMesh(loadedMesh, loadedMaterial);
}

MaterialPtr GameObjectLoader::processMaterial(aiMesh* mesh, const aiScene* scene)
{
    //if (mesh->mMaterialIndex == 0) return nullptr;

    PhongMaterialBuilder phongBuilder;
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    aiColor3D color{0.f,0.f,0.f};
    if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, color))
        phongBuilder.setDiffuseColor(glm::vec3{color.r, color.g, color.b});

    if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, color))
        phongBuilder.setSpecularColor(glm::vec3{color.r, color.g, color.b});

    //if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_TRANSPARENT, color))

    float shininess = 0.0f;
    phongBuilder.setShininess(shininess); // defaults to 0
    if (AI_SUCCESS == material->Get(AI_MATKEY_SHININESS, shininess))
        phongBuilder.setShininess(shininess);

    phongBuilder.setDiffuseMap(loadTexture(material, scene, aiTextureType_DIFFUSE));
    phongBuilder.setSpecularMap(loadTexture(material, scene, aiTextureType_SPECULAR));

    PhongMaterialPtr loadedMaterial = phongBuilder.build();

    int twosided = 0;
    if (AI_SUCCESS == material->Get(AI_MATKEY_TWOSIDED, twosided))
        loadedMaterial->isTwoSided = static_cast<bool>(twosided);

    // Another check to see if the material is transparent
    float opacity = 1.0f;
    if (AI_SUCCESS == material->Get(AI_MATKEY_OPACITY, opacity)) {
        loadedMaterial->opacity = opacity;
        loadedMaterial->isTwoSided = opacity < 1.0f;
    }

    return loadedMaterial;
}

Texture GameObjectLoader::loadTexture(aiMaterial* material, const aiScene* scene, aiTextureType type)
{
    std::map<int, int> aiMapMode2glMapMode{
        {aiTextureMapMode_Wrap, GL_REPEAT},
        {aiTextureMapMode_Clamp, GL_CLAMP_TO_EDGE},
        {aiTextureMapMode_Mirror, GL_MIRRORED_REPEAT},
        {aiTextureMapMode_Decal, GL_REPEAT} // not supported
    };

    // Only one texture supported
    if(material->GetTextureCount(type) != 0) {
        aiString path;
        aiTextureMapMode mapModeU, mapModeV;
        material->Get(AI_MATKEY_TEXTURE(type, 0), path);
        material->Get(AI_MATKEY_MAPPINGMODE_U(type, 0), mapModeU);
        material->Get(AI_MATKEY_MAPPINGMODE_V(type, 0), mapModeV);

        int mapModeS = aiMapMode2glMapMode[mapModeU];
        int mapModeT = aiMapMode2glMapMode[mapModeV];

        const char* texturePath = path.C_Str();
        /* check whether or not this is an embedded texture. If that's the case
         * load it from memory. The name of embedded textures starts with
         * '*' followed by a number that can be used to index scene->mTextures
         * This texture is then converted to bytes read by Texture */
        if (texturePath[0] == '*') {
            aiTexture* texture = scene->mTextures[std::atoi(texturePath + 1)];
            std::uint8_t* textureData = reinterpret_cast<unsigned char*>(texture->pcData);
            if (texture->mHeight == 0)
                return Texture::loadFromMemory(textureData, texture->mWidth, mapModeS, mapModeT);

            else
                return Texture::loadFromMemory(textureData, texture->mWidth * texture->mHeight, mapModeS, mapModeT);

        } else {
			std::filesystem::path path{ texturePath };
			if (path.is_relative())
				path = mWorkingDir / path;

            return Texture::loadFromFile(path.string(),
                                 mapModeS,
                                 mapModeT);
        }
    }

    return Texture{};
}

GameObjectEH GameObjectLoader::fromFile(const std::string& path)
{
	mWorkingDir = (std::filesystem::path{ path }).remove_filename();

	std::cout << mWorkingDir << "\n";

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error loading mesh " << importer.GetErrorString() << "\n";
        return GameObjectEH{};
    }

    return processNode(scene->mRootNode, scene);
}
