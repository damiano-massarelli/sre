#include "GameObjectLoader.h"
#include "Engine.h"
#include "MeshLoader.h"
#include "BlinnPhongMaterial.h"
#include "Transform.h"
#include "SkeletralAnimationControllerComponent.h"
#include "SkeletralAnimationLoader.h"
#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <glm/gtx/matrix_decompose.hpp>

std::map<std::string, Mesh> GameObjectLoader::mMeshCache;

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
        //std::cout << "loading mesh " << mesh->mName.C_Str() << "\n";
        processMesh(go, node, i, mesh, scene);
    }

    for (std::uint32_t i = 0; i < node->mNumChildren; ++i) {

		// bones are also part of the node hierarchy
		// but they should not be processed here
		if (isBone(node->mChildren[i]))
			continue;

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
	glm::vec3 position, scale;
	glm::quat rotation;
	decompose(convertMatrix(node->mTransformation), position, rotation, scale);

    /* There is no need to use local transformations since when the following code is executed
     * all the children have their position already set relative to the parent (thanks to the
     * decomposed matrix). When the following transformations are applied they also affect the
     * children of this node */
    go->transform.setPosition(position);
    go->transform.setRotation(rotation);
    go->transform.setScale(scale);
    return go;
}

#include <iterator>
void GameObjectLoader::processMesh(const GameObjectEH& go, aiNode* node, int meshNumber, aiMesh* mesh, const aiScene* scene)
{
	// creates the cache name for this mesh
	std::string cacheName = mFilePath + std::string{ node->mName.C_Str() } +std::string{ mesh->mName.C_Str() } + std::to_string(meshNumber);

	// loads material
	MaterialPtr loadedMaterial = processMaterial(mesh, scene, cacheName);
	if (loadedMaterial == nullptr) {
		std::cerr << "Mesh " << mesh->mName.C_Str() << "does not have a corresponding material, discarded\n";
		return;
	}

	auto cachedMesh = mMeshCache.find(cacheName);
	if (cachedMesh != mMeshCache.end()) {
		go->addMesh(cachedMesh->second, loadedMaterial);
		return;
	}

    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> texCoords;
	std::vector<std::int32_t> influencingBones;
	std::vector<float> boneWeights;

    std::vector<Vertex> vertices;
    std::vector<std::uint32_t> indices;

    // Load vertex data
    for (std::uint32_t i = 0; i < mesh->mNumVertices; ++i) {
        Vertex v;
		
		auto& bones = getInfluencingBones(i);
		auto& weights = getInfluencingBonesWeights(i);
		influencingBones.insert(influencingBones.end(), bones.begin(), bones.end());
		boneWeights.insert(boneWeights.end(), weights.begin(), weights.end());

        // position
        positions.insert(positions.end(), {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z});
        v.position = glm::vec3{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};

        // normal, if not present 0, 0, 0 is used
        if (mesh->HasNormals()) {
            normals.insert(normals.end(), {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z});
            v.normal = glm::vec3{mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
        } else {
            std::cout << "cannot find normals, setting them to (0, 0, 0)\n";
            normals.insert(normals.end(), {0.0f, 0.0f, 0.0f});
            v.normal = glm::vec3{0.0f, 0.0f, 0.0f};
        }

        // if texture coordinates are not available just put (0, 0)
        if (mesh->mTextureCoords[0]) {
            texCoords.insert(texCoords.end(), {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y});
            v.texCoord = glm::vec2{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        } else {
            std::cout << "cannot find uv coords, setting them to (0, 0)\n";
            texCoords.insert(texCoords.end(), {0.0f, 0.0f});
            v.texCoord = glm::vec2{0.0f, 0.0f};
        }

        vertices.push_back(v);
    }

    // Load indices
    for (std::uint32_t i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];
		for (std::uint32_t j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
    }

    MeshLoader loader;
    loader.loadData(positions.data(), positions.size(), 3);
    loader.loadData(normals.data(), normals.size(), 3);
    loader.loadData(texCoords.data(), texCoords.size(), 2);
	loader.loadData(influencingBones.data(), influencingBones.size(), 4, GL_ARRAY_BUFFER, GL_INT);
	loader.loadData(boneWeights.data(), boneWeights.size(), 4);

    loader.loadData(indices.data(), indices.size(), 0, GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_INT);

    Mesh loadedMesh = loader.getMesh(vertices.size(), indices.size());

	loadedMesh.refCount.onRemove = [cacheName]() { GameObjectLoader::mMeshCache.erase(cacheName); };
	mMeshCache[cacheName] = loadedMesh;
	mMeshCache[cacheName].refCount.setWeak();

    go->addMesh(loadedMesh, loadedMaterial);
}

MaterialPtr GameObjectLoader::processMaterial(aiMesh* mesh, const aiScene* scene, const std::string& cacheName)
{
    //if (mesh->mMaterialIndex == 0) return nullptr;

    BlinnPhongMaterialBuilder phongBuilder;
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

    phongBuilder.setDiffuseMap(loadTexture(material, scene, aiTextureType_DIFFUSE, cacheName));
    phongBuilder.setSpecularMap(loadTexture(material, scene, aiTextureType_SPECULAR, cacheName));

    BlinnPhongMaterialPtr loadedMaterial = phongBuilder.build();

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

Texture GameObjectLoader::loadTexture(aiMaterial* material, const aiScene* scene, aiTextureType type, const std::string& meshCacheName)
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
				return Texture::loadFromMemoryCached(meshCacheName + std::string{ texturePath }, textureData, texture->mWidth, mapModeS, mapModeT);
            else
                return Texture::loadFromMemoryCached(meshCacheName + std::string{ texturePath }, textureData, texture->mWidth * texture->mHeight, mapModeS, mapModeT);

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

void GameObjectLoader::decompose(const glm::mat4& mat, glm::vec3& outPos, glm::quat& outRot, glm::vec3& outScale)
{
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(mat, outScale, outRot, outPos, skew, perspective);
}

void GameObjectLoader::findBones(const aiScene* scene)
{
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[i];
		for (unsigned int j = 0; j < mesh->mNumBones; j++) {
			aiBone* bone = mesh->mBones[j];
			Bone b;
			b.offset = convertMatrix(bone->mOffsetMatrix);

			// this is not very cache friendly but for now it is ok
			auto boneName = std::string{ bone->mName.C_Str() };
			auto boneIndex = mBones.size();
			mBoneName2Index[boneName] = boneIndex;
			std::cout << "setting " << boneName << " " << boneIndex << "\n";
			mBones.push_back(b);

			// loads weights for the vertices influenced by this bone
			for (unsigned int k = 0; k < bone->mNumWeights; k++) {
				auto vertexIndex = bone->mWeights[k].mVertexId;
				float weight = bone->mWeights[k].mWeight;
				mVertexToInfluencingBones[vertexIndex].push_back(boneIndex);
				mVertexToInfluencingBonesWeights[vertexIndex].push_back(weight);
			}
		}
	}
}

void GameObjectLoader::buildBonesHierarchy(const aiNode* node)
{
	// build the bone hierarchy recursively
	if (isBone(node)) {
		std::uint32_t index = mBoneName2Index[node->mName.C_Str()];
		auto& bone = mBones[index];
		bone.name = node->mName.C_Str();
		bone.toParentSpace = convertMatrix(node->mTransformation);
		decompose(convertMatrix(node->mTransformation), bone.position, bone.rotation, bone.scale);

		if (isBone(node->mParent))
			bone.parent = mBoneName2Index[node->mParent->mName.C_Str()];
		else
			bone.parent = -1;
		for (unsigned int i = 0; i < node->mNumChildren; ++i) {
			aiNode* child = node->mChildren[i];
			if (isBone(child))
				bone.children.push_back(mBoneName2Index[child->mName.C_Str()]);
			buildBonesHierarchy(child);
		}
	}
	else {
		for (unsigned int i = 0; i < node->mNumChildren; ++i)
			buildBonesHierarchy(node->mChildren[i]);
	}
}

bool GameObjectLoader::isBone(const aiNode* node)
{
	if (node == nullptr) return false;
	return mBoneName2Index.find(node->mName.C_Str()) != mBoneName2Index.end();
}


std::vector<std::uint32_t>& GameObjectLoader::getInfluencingBones(std::uint32_t vertexIndex)
{
	auto& bones = mVertexToInfluencingBones[vertexIndex];
	if (bones.size() > 4) std::cerr << "more than 4 bones per vertex, not supported\n";
	bones.resize(MAX_BONES_PER_VERTEX, 0);

	return bones;
}

std::vector<float>& GameObjectLoader::getInfluencingBonesWeights(std::uint32_t vertexIndex)
{
	auto& weights = mVertexToInfluencingBonesWeights[vertexIndex];
	weights.resize(MAX_BONES_PER_VERTEX, 0.0f);

	return weights;
}

#include <functional>

GameObjectEH GameObjectLoader::fromFile(const std::string& path)
{
	mFilePath = path;
	mWorkingDir = (std::filesystem::path{ path }).remove_filename();

    Assimp::Importer importer;
	//importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error loading mesh " << importer.GetErrorString() << "\n";
        return GameObjectEH{};
    }

	std::function<void(Bone&)> f;
	f = [this, &f](Bone& b) {
		std::cout << b.name << ":\n\t";
		for (auto& i : b.children)
			std::cout << mBones[i].name << " ";
		std::cout << "\n\n";

		for (auto& i : b.children)
			f(mBones[i]);
	};

	findBones(scene);
	buildBonesHierarchy(scene->mRootNode);

/*	f(mBones[0]);*/

// 	std::cout << scene->mNumAnimations << "\n";
// 	for (auto i = 0; i < scene->mNumAnimations; ++i) {
// 		std::cout << "name: " << scene->mAnimations[i]->mName.C_Str() << "\n";
// 	}
// 	std::cout << scene->mAnimations[2]->mDuration << "\n";
// 	std::cout << scene->mAnimations[2]->mTicksPerSecond << "\n";

// 	aiAnimation* anim = scene->mAnimations[1];
// 	for (int i = 0; i < anim->mNumChannels; ++i) {
// 		auto ch = anim->mChannels[i];
// 		std::cout << "pos " << ch->mNumPositionKeys << "\n";
// 		std::cout << "rot " << ch->mNumRotationKeys << "\n";
// 		std::cout << "sca " << ch->mNumScalingKeys << "\n";
// 	}

	SkeletralAnimationLoader loader;
	auto animation = loader.fromAssimpScene(scene, mBoneName2Index);

    auto go = processNode(scene->mRootNode, scene);
	go->addComponent(std::make_shared<SkeletralAnimationControllerComponent>(go, mBones));
	go->getComponent<SkeletralAnimationControllerComponent>()->animation = animation;

	std::cout << "num meshes " << scene->mNumMeshes << "\n";

	return go;
}
