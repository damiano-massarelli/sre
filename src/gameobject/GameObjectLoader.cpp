#include "GameObjectLoader.h"
#include "Engine.h"
#include "MeshLoader.h"
#include "PhongMaterial.h"
#include "Transform.h"
#include <iostream>
#include <cstdint>

GameObjectEH GameObjectLoader::processNode(aiNode* node, const aiScene* scene)
{
    GameObjectEH go = Engine::renderSys.createGameObject();

    std::cout << "at node " << node->mName.C_Str() << "\n";
    for (std::uint32_t i = 0; i < node->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        std::cout << "loading mesh " << mesh->mName.C_Str() << "\n";
        processMesh(go, mesh, scene);
    }

    for (std::uint32_t i = 0; i < node->mNumChildren; ++i) {
        go->transform.addChild(processNode(node->mChildren[i], scene));
    }

    return go;
}

void GameObjectLoader::processMesh(const GameObjectEH& go, aiMesh* mesh, const aiScene* scene)
{
    // data regarding vertices: positions, normals, texture coords
    std::vector<float> vertexData;
    std::vector<Vertex> vertices;
    std::vector<std::uint32_t> indices;

    // Load vertex data
    for (std::uint32_t i = 0; i < mesh->mNumVertices; ++i) {
        Vertex v;

        // position
        vertexData.insert(vertexData.end(), {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z});
        v.position = glm::vec3{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};

        // normal
        vertexData.insert(vertexData.end(), {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z});
        v.normal = glm::vec3{mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};

        // if texture coordinates are not available just put (0, 0)
        if (mesh->mTextureCoords[0]) {
            vertexData.insert(vertexData.end(), {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y});
            v.texCoord = glm::vec2{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        } else {
            vertexData.insert(vertexData.end(), {0.0f, 0.0f});
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

    Mesh loadedMesh = MeshLoader::createMesh(vertexData.data(), vertices.size(), indices.data(), indices.size());
    MaterialPtr loadedMaterial = processMaterial(mesh, scene);
    if (loadedMaterial == nullptr) {
        std::cerr << "Mesh " << mesh->mName.C_Str() << "does not have a corresponding material, discarded\n";
        return;
    }

    go->addMesh(loadedMesh, loadedMaterial);
}

MaterialPtr GameObjectLoader::processMaterial(aiMesh* mesh, const aiScene* scene)
{
    if (mesh->mMaterialIndex == 0) return nullptr;

    PhongMaterialBuilder phongBuilder;
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    if (material->GetTextureCount(aiTextureType_DIFFUSE)) {
        aiString path;
        material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
        phongBuilder.setDiffuseMap(std::string{path.C_Str()});
    }

    if (material->GetTextureCount(aiTextureType_SPECULAR)) {
        aiString path;
        material->GetTexture(aiTextureType_SPECULAR, 0, &path);
        phongBuilder.setDiffuseMap(std::string{path.C_Str()});
    }

    aiColor3D color{0.f,0.f,0.f};
    if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, color))
        phongBuilder.setDiffuseColor(glm::vec3{color.r, color.g, color.b});

    if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, color))
        phongBuilder.setSpecularColor(glm::vec3{color.r, color.g, color.b});

    return phongBuilder.build();
}

GameObjectEH GameObjectLoader::fromFile(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_GenSmoothNormals | aiProcess_Triangulate);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error loading mesh " << importer.GetErrorString() << "\n";
        return GameObjectEH{};
    }

    return processNode(scene->mRootNode, scene);
}
