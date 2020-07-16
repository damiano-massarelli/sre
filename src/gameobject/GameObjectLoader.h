#ifndef GAMEOBJECTLOADER_H
#define GAMEOBJECTLOADER_H
#include "gameobject/GameObject.h"
#include "rendering/mesh/Mesh.h"
#include "rendering/materials/Material.h"
#include "rendering/materials/Texture.h"
#include "skeletalAnimation/Bone.h"
#include "skeletalAnimation/SkeletalAnimation.h"
#include "skeletalAnimation/SkeletralAnimationControllerComponent.h"
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>
#include <map>

/**
  * A GameObjectLoader is used to create a GameObject form a model stored on a file. */
class GameObjectLoader
{
    private:
		/** Max number of bones that can influence a vertex */
		static constexpr int MAX_BONES_PER_VERTEX = 4;

		/** A cache for meshes */
		static std::map<std::string, Mesh> mMeshCache;

        /** The current working directory */
		std::filesystem::path mWorkingDir;

		/** The path of the file we are working on */
		std::string mFilePath;

		/** List of bones of this model */
		std::vector<Bone> mBones;

		/** maps the name of a bone to an index of mBones */
		std::map<std::string, std::uint32_t> mBoneName2Index;

		/** for every vertex the indices (of mBones) of the bones influencing it */
		std::map<std::pair<std::uint32_t, aiMesh*>, std::vector<std::uint32_t>> mVertexToInfluencingBones;

		/** for every vertex the weights of the bones influencing it */
		std::map<std::pair<std::uint32_t, aiMesh*>, std::vector<float>> mVertexToInfluencingBonesWeights;

		/** skeletal animation controller to add to animated materials */
		std::shared_ptr<SkeletalAnimationControllerComponent> mSkeletalAnimationController = nullptr;

        GameObjectEH processNode(aiNode* node, const aiScene* scene);
        void processMesh(const GameObjectEH& go, aiNode* node, int meshNumber, aiMesh* mesh, const aiScene* scene);
        MaterialPtr processMaterial(aiMesh* mesh, const aiScene* scene, const std::string& cacheName);
		Texture loadTexture(aiMaterial* material, const aiScene* scene, aiTextureType type, int id, const std::string& meshCacheName);

		void decompose(const glm::mat4& mat, glm::vec3& outPos, glm::quat& outRot, glm::vec3& outScale);

		/**
		 * finds all the bones and stores them in the mBones.
		 * A mapping between bones' names and their indices in mBones 
		 * is kept in mBoneName2Index.
		 */
		void findBones(const aiScene* scene);

		/**
		 * Creates the actual hierarchy of bones.
		 * To do so, nodes are read from the root node
		 * only considering those for which isBone is true
		 */
		void buildBonesHierarchy(const aiNode* node);

		/**
		 * Checks whether a node is a bone.
		 * Call this method after findBones.
		 */
		bool isBone(const aiNode* node);

		void computeTangentsAndBitangentSign(const aiMesh* mesh, std::vector<float>& tangents, std::vector<std::int32_t> bitangentSign) const;

		std::vector<std::uint32_t>& getInfluencingBones(std::uint32_t vertexIndex, aiMesh* mesh);

		std::vector<float>& getInfluencingBonesWeights(std::uint32_t vertexIndex, aiMesh* mesh);


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
		  * If the loaded model has bones a SkeletalAnimationControllerComponent is added
		  * as its component. If the model also contains a skeletal animation it is added
		  * to the SkeletalAnimationControllerComponent and its name is "default"
		  * (@see SkeletalAnimationControllerComponent::playAnimation )
          * @param path the path of the model to load
          * @return a reference to the root GameObject (invalid reference if the model could not be loaded)
          * @see GameObjectEH
          * @see GameObjectEH::isValid
          * @see Transform::getChildren
		  * @see SkeletalAnimationControllerComponent */
        GameObjectEH fromFile(const std::string& path);
};

#endif // GAMEOBJECTLOADER_H
