#ifndef GAMEOBJECTLOADER_H
#define GAMEOBJECTLOADER_H
#include "GameObject.h"
#include "Mesh.h"
#include "Material.h"
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class GameObjectLoader
{
    private:
        static GameObjectEH processNode(aiNode* node, const aiScene* scene);
        static void processMesh(const GameObjectEH& go, aiMesh* mesh, const aiScene* scene);
        static MaterialPtr processMaterial(aiMesh* mesh, const aiScene* scene);

    public:
        static GameObjectEH fromFile(const std::string& path);
};

#endif // GAMEOBJECTLOADER_H
