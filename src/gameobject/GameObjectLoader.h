#ifndef GAMEOBJECTLOADER_H
#define GAMEOBJECTLOADER_H
#include "GameObject.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

/**
  * A GameObjectLoader is used to create a GameObject form a model stored on a file. */
class GameObjectLoader
{
    private:
        /** The current working directory */
        //std::filesystem::path mWorkingDir;

        GameObjectEH processNode(aiNode* node, const aiScene* scene);
        void processMesh(const GameObjectEH& go, aiMesh* mesh, const aiScene* scene);
        MaterialPtr processMaterial(aiMesh* mesh, const aiScene* scene);
        Texture loadTexture(aiMaterial* material, const aiScene* scene, aiTextureType type);

    public:
        /**
          * If true vertex data is stored into the loaded meshes
          * Data can be queried using Mesh::getVertexData */
        bool keepVertexData = false;

        /** Creates a GameObjectLoader */
        GameObjectLoader() = default;

        /**
          * Loads a GameObject from a model stored on file.
          * The hierarchy of models in the file is represented with
          * a hierarchy of GameObjects.
          * @param path the path of the model to load
          * @return a reference to the root GameObject (invalid reference if the model could not be loaded)
          * @sa GameObjectEH
          * @sa GameObjectEH::isValid
          * @sa Transform::getChildren */
        GameObjectEH fromFile(const std::string& path);
};

#endif // GAMEOBJECTLOADER_H
