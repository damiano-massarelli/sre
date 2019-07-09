#include "GameObjectRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Engine.h"
#include <map>
#include <tuple>
#include <glad/glad.h>
#include <unordered_map>

void GameObjectRenderer::draw(const Mesh* mesh)
{
    glBindVertexArray(mesh->mVao);

    if (mesh->mUsesIndices)
        glDrawElements(mesh->mDrawMode, mesh->mIndicesNumber, GL_UNSIGNED_INT, (void *)0);
    else
        glDrawArrays(mesh->mDrawMode, 0, mesh->mVertexNumber);

    glBindVertexArray(0);
}

void GameObjectRenderer::drawMeshes(const std::unordered_map<Material*, std::vector<DrawData>>& material2mesh)
{

	for (const auto&[material, drawData] : material2mesh) {
		if (material->needsOrderedRendering()) {

		}
		else {
			material->use();
			for (const auto& data : drawData) {
				material->shader.setMat4(material->getModelLocation(), data.toWorld);
				material->shader.setMat3(material->getNormalModelLocation(), data.toWorldForNormals);
				draw(data.mesh);
			}

			material->after();
		}
	}

		/*
		const auto& modelToWorld = go.transform.modelToWorld();
		auto normalModel = glm::inverse(glm::transpose(glm::mat3(modelToWorld)));

		if (material->needsOrderedRendering()) {
			float order = material->renderOrder(go.transform.getPosition());
			orderedRender[order] = std::make_tuple(mesh, material, modelToWorld, normalModel);
			continue;
		}
		else {
			material->use();
			material->shader.setMat4(mat->getModelLocation(), go.transform.modelToWorld());
			material->shader.setMat3(mat->getNormalModelLocation(), normalModel);
			draw(mesh);
			material->after();
		}
	}

	// render meshed that need to be rendered in a specific order
	for (auto ord = orderedRender.rbegin(); ord != orderedRender.rend(); ++ord) {
		auto&[mesh, mat, model, normalModel] = ord->second;
		mat->use();
		mat->shader.setMat4(mat->getModelLocation(), model);
		mat->shader.setMat3(mat->getNormalModelLocation(), normalModel);
		draw(mesh);
		mat->after();
	}
	*/
}

void GameObjectRenderer::render()
{
    std::map<float, std::tuple<Mesh, MaterialPtr, glm::mat4, glm::mat4>> orderedRender;

	std::unordered_map<Material*, std::vector<DrawData>> material2mesh;

	for (auto& go : Engine::gameObjectManager.mGameObjects) {
		for (std::size_t meshIndex = 0; meshIndex < go.mMeshes.size(); meshIndex++) {

			Material* meshMaterial = go.mMaterials[meshIndex].get();
			meshMaterial = mForcedMaterial ? mForcedMaterial.get() : meshMaterial;

			const Mesh* mesh = &(go.mMeshes[meshIndex]);

			// do not render this mesh if its material does not support the current render phase
			if ((meshMaterial->unSupportedRenderPhases & Engine::renderSys.getRenderPhase()))
				continue;

			auto it = material2mesh.find(meshMaterial);
			if (it == material2mesh.end()) {
				material2mesh[meshMaterial] = { {mesh, go.transform.modelToWorld(), go.transform.modelToWorldForNormals()} };
				material2mesh[meshMaterial].reserve(50);
			}
			else
				it->second.push_back({ mesh, go.transform.modelToWorld(), go.transform.modelToWorldForNormals() });
		}
	}

	drawMeshes(material2mesh);
}

void GameObjectRenderer::forceMaterial(const MaterialPtr& material)
{
	mForcedMaterial = material;
}
