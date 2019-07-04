#include "RenderSystem.h"
#include "Shader.h"
#include "Light.h"
#include "Engine.h"
#include "MeshLoader.h"
#include "ShadowMapMaterial.h"
#include "MeshCreator.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <exception>
#include <algorithm>
#include <iostream>
#include <map>

RenderSystem::RenderSystem()
{
    camera = Engine::gameObjectManager.createGameObject();
    camera->name = "defaultCamera";
}

void RenderSystem::createWindow(std::uint32_t width, std::uint32_t height, float fovy, float nearPlane, float farPlane)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "Cannot init SDL " << SDL_GetError() << "\n";
        std::terminate();
    }

    SDL_GL_LoadLibrary(nullptr); // use default OpenGL
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

//	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
//	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    mWindow = SDL_CreateWindow("opengl", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    if (mWindow == nullptr) {
        std::cout << "Cannot create window " << SDL_GetError() << "\n";
        std::terminate();
    }

    SDL_GL_CreateContext(mWindow);

    // Use v-sync
    //SDL_GL_SetSwapInterval(1);

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        std::terminate();
    }

    initGL(width, height, fovy, nearPlane, farPlane);
	initDeferredRendering();
	mEffectFBO.init(width, height);
	effectManager.init();
	initShadowFbo();
	fogSettings.init();
	shadowMappingSettings.init();
}

void RenderSystem::initGL(std::uint32_t width, std::uint32_t height, float fovy, float nearPlane, float farPlane)
{
    mProjection = glm::perspective(fovy, static_cast<float>(width)/height, nearPlane, farPlane);
	mNearPlane = nearPlane;
	mFarPlane = farPlane;
	mVerticalFov = fovy;

	mInvertView = glm::mat4	{
			glm::vec4{ -1, 0, 0, 0 },
			glm::vec4{ 0, 1, 0, 0 },
			glm::vec4{ 0, 0, -1, 0 },
			glm::vec4{ 0, 0, 0, 1 }};

    /* Uniform buffer object set up for common matrices */
    glGenBuffers(1, &mUboCommonMat);
    glBindBuffer(GL_UNIFORM_BUFFER, mUboCommonMat);
	// 3 matrices, view projection and shadow mapping projection
    glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, COMMON_MAT_UNIFORM_BLOCK_INDEX, mUboCommonMat);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBuffer(GL_UNIFORM_BUFFER, mUboCommonMat);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mProjection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    /* Uniform buffer object set up for lights */
    glGenBuffers(1, &mUboLights);
    glBindBuffer(GL_UNIFORM_BUFFER, mUboLights);
    // 16 numLights, 112 size of a light array element
    glBufferData(GL_UNIFORM_BUFFER, 16 + 128 * MAX_LIGHT_NUMBER, nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, LIGHT_UNIFORM_BLOCK_INDEX, mUboLights);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    /* Uniform buffer object set up for camera */
    glGenBuffers(1, &mUboCamera);
    glBindBuffer(GL_UNIFORM_BUFFER, mUboCamera);

    // size for position and direction
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::vec3), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, CAMERA_UNIFORM_BLOCK_INDEX, mUboCamera);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    /* General OpenGL settings */
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

void RenderSystem::initDeferredRendering()
{
	deferredRenderingFBO.init(getScreenWidth(), getScreenHeight());

	mDirectionalLightDeferred = Shader::loadFromFile({ "shaders/deferred_rendering/directionalLightVS.glsl" },
		std::vector<std::string>{},
		{ "shaders/Light.glsl", "shaders/ShadowMappingCalculation.glsl", "shaders/deferred_rendering/directionalLightFS.glsl" });

	mDirectionalLightDeferred.use();
	mDirectionalLightDeferred.setInt("DiffuseData", 0);
	mDirectionalLightDeferred.setInt("SpecularData", 1);
	mDirectionalLightDeferred.setInt("PositionData", 2);
	mDirectionalLightDeferred.setInt("NormalData", 3);
	mDirectionalLightDeferred.setInt("shadowMap", 15);
	mDirectionalLightDeferred.bindUniformBlock("CommonMat", RenderSystem::COMMON_MAT_UNIFORM_BLOCK_INDEX);
	mDirectionalLightDeferred.bindUniformBlock("Lights", RenderSystem::LIGHT_UNIFORM_BLOCK_INDEX);
	mDirectionalLightDeferred.bindUniformBlock("Camera", RenderSystem::CAMERA_UNIFORM_BLOCK_INDEX);
	mDirectionalLightDeferred.bindUniformBlock("ShadowMapParams", RenderSystem::SHADOWMAP_UNIFORM_BLOCK_INDEX);

	mDirectionalLightDeferredLightIndexLocation = mDirectionalLightDeferred.getLocationOf("lightIndex");

	MeshLoader loader;
	float verts[]{ -1, -1, -1, 1, 1, 1, 1, -1 };
	float texCoords[]{ 0, 0, 0, 1, 1, 1, 1, 0 };
	std::uint32_t indices[]{ 0, 2, 1, 0, 3, 2 };
	loader.loadData(verts, 8, 2);
	loader.loadData(texCoords, 8, 2);
	loader.loadData(indices, 6, 0, GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_INT);
	mScreenMesh = loader.getMesh(0, 6);


	mPointLightDeferred = Shader::loadFromFile({ "shaders/Light.glsl", "shaders/deferred_rendering/pointLightVS.glsl" },
		std::vector<std::string>{},
		{ "shaders/Light.glsl", "shaders/deferred_rendering/pointLightFS.glsl" });

	mPointLightDeferred.use();
	mPointLightDeferred.setInt("DiffuseData", 0);
	mPointLightDeferred.setInt("SpecularData", 1);
	mPointLightDeferred.setInt("PositionData", 2);
	mPointLightDeferred.setInt("NormalData", 3);
	mPointLightDeferred.bindUniformBlock("Lights", RenderSystem::LIGHT_UNIFORM_BLOCK_INDEX);
	mPointLightDeferred.bindUniformBlock("Camera", RenderSystem::CAMERA_UNIFORM_BLOCK_INDEX);
	mPointLightDeferredLightIndexLocation = mPointLightDeferred.getLocationOf("lightIndex");

	mPointLightSphere = MeshCreator::sphere(1.0f, 10, 10, false, false);

	mPointLightDeferredStencil = Shader::loadFromFile({ "shaders/Light.glsl", "shaders/deferred_rendering/pointLightSphereStencilPassVS.glsl" },
		std::vector<std::string>{},
		{ "shaders/deferred_rendering/pointLightSphereStencilPassFS.glsl" });
	mPointLightDeferredStencil.use();
	mPointLightDeferredStencil.bindUniformBlock("Lights", RenderSystem::LIGHT_UNIFORM_BLOCK_INDEX);
	mPointLightStencilLightIndexLocation = mPointLightDeferredStencil.getLocationOf("lightIndex");
	mPointLightStencilScaleLocation = mPointLightDeferredStencil.getLocationOf("scale");
}

void RenderSystem::initShadowFbo()
{
	mShadowMapMaterial = std::make_shared<ShadowMapMaterial>();

	mShadowMap = Texture::load(nullptr, shadowMappingSettings.mapWidth, shadowMappingSettings.mapHeight, GL_REPEAT, GL_REPEAT, false, GL_DEPTH_COMPONENT, GL_FLOAT);

	glGenFramebuffers(1, &mShadowFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, mShadowFbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mShadowMap.getId(), 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Shadow map frame buffer is incomplete\n";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderSystem::updateLights()
{
    std::size_t numLight = std::min((std::size_t)MAX_LIGHT_NUMBER, mLights.size());
    glBindBuffer(GL_UNIFORM_BUFFER, mUboLights);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(std::size_t), (void*)&numLight);
    for (std::size_t i = 0; i < numLight; i++) {
        int base = 16 + 128 * i;
        LightPtr lightComponent = mLights[i]->getComponent<Light>();
        if (lightComponent == nullptr) continue;

        std::uint32_t lightType = static_cast<std::uint32_t>(lightComponent->type);
        glm::vec3 attenuation = glm::vec3{
            lightComponent->attenuationConstant,
            lightComponent->attenuationLinear,
            lightComponent->attenuationQuadratic
        };

        glm::vec2 spotAngles{
            glm::cos(lightComponent->innerAngle),
            glm::cos(lightComponent->outerAngle)
        };

        // type
        glBufferSubData(GL_UNIFORM_BUFFER, base, sizeof(std::uint32_t), (void *)(&lightType));

        // position
        glBufferSubData(GL_UNIFORM_BUFFER, base + 16, sizeof(glm::vec3), glm::value_ptr(mLights[i]->transform.getPosition()));

        // direction
        glBufferSubData(GL_UNIFORM_BUFFER, base + 32, sizeof(glm::vec3), glm::value_ptr(mLights[i]->transform.forward()));

        // ambient
        glBufferSubData(GL_UNIFORM_BUFFER, base + 48, sizeof(glm::vec3), glm::value_ptr(lightComponent->ambientColor));

        // diffuse
        glBufferSubData(GL_UNIFORM_BUFFER, base + 64, sizeof(glm::vec3), glm::value_ptr(lightComponent->diffuseColor));

        // specular
        glBufferSubData(GL_UNIFORM_BUFFER, base + 80, sizeof(glm::vec3), glm::value_ptr(lightComponent->specularColor));

        // attenuation
        glBufferSubData(GL_UNIFORM_BUFFER, base + 96, sizeof(glm::vec3), glm::value_ptr(attenuation));

        // spot light angles
        glBufferSubData(GL_UNIFORM_BUFFER, base + 112, sizeof(glm::vec2), glm::value_ptr(spotAngles));
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void RenderSystem::updateCamera()
{
    glm::vec3 cameraPosition{0.0f};
    glm::vec3 cameraDirection{0.0f, 0.0f, 1.0f};
    if (camera) {
        cameraPosition = camera->transform.getPosition();
        cameraDirection = camera->transform.forward();
    }

    glBindBuffer(GL_UNIFORM_BUFFER, mUboCamera);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec3), glm::value_ptr(cameraPosition));
    glBufferSubData(GL_UNIFORM_BUFFER, 16, sizeof(glm::vec3), glm::value_ptr(cameraDirection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

glm::mat4 RenderSystem::getViewMatrix(const Transform& transform)
{
	glm::mat4 view = glm::mat4{ 1.0f };
	view = glm::translate(glm::mat4{ 1.0f }, -transform.getPosition());
	glm::mat4 inverseRotation = glm::transpose(glm::toMat4(transform.getRotation()));
	view = inverseRotation * view;

	/* Inverts the view so that we watch in the direction of camera->transform.forward() */
	return mInvertView * view;
}

void RenderSystem::prepareDeferredRendering()
{
	if (shadowMappingSettings.getShadowStrength() != 0.0f)
		renderShadows();

	// view port might be changed during shadow rendering
	glViewport(0, 0, getScreenWidth(), getScreenHeight());
	glEnable(GL_DEPTH_TEST);

	// bind the deferred rendering frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, deferredRenderingFBO.getFBO());

	// set up the stencil test so that only the parts affected by deferred rendering
	// are actually lit in the deferred rendering directional light pass pass
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilMask(0xFF);

	// clear all the buffers
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    /* Camera calculations */
    glm::mat4 view = glm::mat4{ 1.0f };
	if (camera) 
		view = getViewMatrix(camera->transform);

    /* Sets the camera matrix to a UBO so that it is shared */
	// TODO move to a dedicated method
	glBindBuffer(GL_UNIFORM_BUFFER, mUboCommonMat);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mProjection));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

    updateLights();
    updateCamera();
}

void RenderSystem::renderScene()
{
	prepareDeferredRendering();
	render(RenderPhase::DEFERRED_RENDERING);
	finalizeDeferredRendering();

	render(RenderPhase::FORWARD_RENDERING);
	finalizeRendering();
}

void RenderSystem::render(RenderPhase phase)
{
	mRenderPhase = phase;
	Engine::gameObjectRenderer.render(Engine::gameObjectManager.getGameObjects());
}

void RenderSystem::finalizeDeferredRendering()
{
	/* Depth and stencil information is needed in the forward rendering pass (see renderScene).
	 * Therefore, we need to copy the depth and stencil information created during the deferred
	 * shader pass into the currently bound fbo. */
	glBindFramebuffer(GL_READ_FRAMEBUFFER, deferredRenderingFBO.getFBO());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mEffectFBO.getFbo());
	glBlitFramebuffer(
		0, 0, deferredRenderingFBO.getWidth(), deferredRenderingFBO.getHeight(), 0, 0, getScreenWidth(), getScreenHeight(),
		GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST
	);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, mEffectFBO.getFbo());

	// clear the color buffer of the currently bound fbo (can be either effects fbo or default (0) fbo)
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// we are rendering to a texture now (or screen)
	// there is no need of using a depth buffer
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	// stop writing into the stencil buffer
	glStencilMask(0);
	glDisable(GL_STENCIL_TEST);

	// bind texture used by directional and point light passes
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, deferredRenderingFBO.getDiffuseBuffer().getId());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, deferredRenderingFBO.getSpecularBuffer().getId());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, deferredRenderingFBO.getPositionBuffer().getId());
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, deferredRenderingFBO.getNormalBuffer().getId());
	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_2D, mShadowMap.getId());

	// perform directional light pass (include shadows)
	directionalLightPass();

	// perform point light pass
	pointLightPass();

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
}

void RenderSystem::finalizeRendering()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // unbind effects frame buffer

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	effectManager.mPostProcessingShader.use();
	effectManager.update();
	glBindVertexArray(mScreenMesh.mVao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mEffectFBO.getColorBuffer().getId());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mEffectFBO.getDepthBuffer().getId());
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)0);

	glEnable(GL_DEPTH_TEST);

	SDL_GL_SwapWindow(mWindow);
}

void RenderSystem::stencilPass(int lightIndex, float radius)
{
	// different for every sphere
	glClear(GL_STENCIL_BUFFER_BIT);

	// both faces of the sphere must be rendered
	glDisable(GL_CULL_FACE);
	// depth test must be enabled
	glEnable(GL_DEPTH_TEST);

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	// increment if back face of sphere is behind something
	glStencilOpSeparate(GL_BACK, GL_REPLACE, GL_REPLACE, GL_KEEP);
	// decrement if front face of sphere is before something
	glStencilOpSeparate(GL_FRONT, GL_REPLACE, GL_REPLACE, GL_KEEP);

	// do not write this sphere on the color buffer
	glDrawBuffer(GL_NONE);
	mPointLightDeferredStencil.use();
	mPointLightDeferredStencil.setFloat(mPointLightStencilScaleLocation, radius);
	mPointLightDeferredStencil.setInt(mPointLightStencilLightIndexLocation, lightIndex);

	glBindVertexArray(mPointLightSphere.mVao);
	glDrawElements(GL_TRIANGLES, mPointLightSphere.mIndicesNumber, GL_UNSIGNED_INT, (void *)0);

	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void RenderSystem::pointLightPass()
{
	glStencilMask(0xFF);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	for (int i = 0; i < mLights.size(); i++) {
		if (mLights[i]->getComponent<Light>()->type != Light::Type::POINT)
			continue;
		float radius = computePointLightRadius(mLights[i]->getComponent<Light>());

		stencilPass(i, radius);

		glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
		mPointLightDeferred.use();
		mPointLightDeferred.setInt(mPointLightDeferredLightIndexLocation, i);
		glBindVertexArray(mScreenMesh.mVao);
		glDrawElements(GL_TRIANGLES, mPointLightSphere.mIndicesNumber, GL_UNSIGNED_INT, (void *)0);
	}

	glDisable(GL_BLEND);
	glDisable(GL_STENCIL_TEST);
}

void RenderSystem::directionalLightPass()
{
	glBindVertexArray(mScreenMesh.mVao);

	// enable stencil test so that this operation is only carried
	// out for those pixels actually drawn during deferred rendering
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_EQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	mDirectionalLightDeferred.use();

 	for (int i = 0; i < mLights.size(); i++) {
		if (mLights[i]->getComponent<Light>()->type != Light::Type::DIRECTIONAL)
			continue;
		mDirectionalLightDeferred.setInt(mDirectionalLightDeferredLightIndexLocation, i);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)0);
	}

	glDisable(GL_STENCIL_TEST);
}

void RenderSystem::renderShadows()
{
	if (mLights.size() == 0)
		return;

	auto light = mLights[0]->getComponent<Light>();
	if (!light->castShadow || light->type != Light::Type::DIRECTIONAL)  return;

	glm::mat4 lightProjection = glm::mat4{ 0.0f };
	lightProjection[0][0] = 2.0f / shadowMappingSettings.width;
	lightProjection[1][1] = 2.0f / shadowMappingSettings.height;
	lightProjection[2][2] = -2.0f / shadowMappingSettings.depth;
	lightProjection[3][3] = 1.0f;
// 	glm::mat4 lightProjection = glm::ortho(-shadowMappingSettings.width / 2, shadowMappingSettings.width / 2,
// 		-shadowMappingSettings.height / 2, shadowMappingSettings.height / 2,
// 		0.1f, shadowMappingSettings.depth);

	glm::mat4 lightView = getViewMatrix(mLights[0]->transform);

	glm::mat4 lightSpace = lightProjection * lightView;

	glBindBuffer(GL_UNIFORM_BUFFER, mUboCommonMat);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(lightProjection));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(lightView));
	glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(lightSpace));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glViewport(0, 0, shadowMappingSettings.mapWidth, shadowMappingSettings.mapHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, mShadowFbo);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);

	if (shadowMappingSettings.useFastShader)
		Engine::gameObjectRenderer.forceMaterial(mShadowMapMaterial);
	render(RenderPhase::SHADOW_MAPPING);
	if (shadowMappingSettings.useFastShader)
		Engine::gameObjectRenderer.forceMaterial(nullptr);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Bind texture so that it is available
	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_2D, mShadowMap.getId());
}

float RenderSystem::computePointLightRadius(const std::shared_ptr<Light>& light) const
{
	float linear = light->attenuationLinear;
	float constant = light->attenuationConstant;
	float quadratic = light->attenuationQuadratic;
	float lightMax = std::max({ light->diffuseColor.r, light->diffuseColor.g , light->diffuseColor.b,
		light->specularColor.r, light->specularColor.g, light->specularColor.b });
	float radius = (-linear + std::sqrtf(linear * linear - 4 * quadratic * (constant - (256.0 / 5.0) * lightMax))) / (2 * quadratic);

	return radius;
}

void RenderSystem::addLight(const GameObjectEH& light)
{
    if (light->getComponent<Light>() == nullptr) {
        std::cerr << "Adding a light without a Light component, discarded\n";
        return;
    }

    mLights.push_back(light);
}

std::int32_t RenderSystem::getScreenWidth() const
{
	std::int32_t w;
	SDL_GetWindowSize(mWindow, &w, nullptr);
	return w;
}

std::int32_t RenderSystem::getScreenHeight() const
{
	std::int32_t h;
	SDL_GetWindowSize(mWindow, nullptr, &h);
	return h;
}

float RenderSystem::getNearPlane() const
{
	return mNearPlane;
}

float RenderSystem::getFarPlane() const
{
	return mFarPlane;
}

float RenderSystem::getVerticalFov() const
{
	return mVerticalFov;
}

RenderPhase RenderSystem::getRenderPhase() const
{
	return mRenderPhase;
}

void RenderSystem::cleanUp()
{
    // Delete uniform buffers
    glDeleteBuffers(1, &mUboCommonMat);
    glDeleteBuffers(1, &mUboLights);
	glDeleteFramebuffers(1, &mShadowFbo);
	mShadowMapMaterial = nullptr;

	effectManager.cleanUp();

	// cleans shaders
	mPointLightDeferred = Shader();
	mPointLightDeferredStencil = Shader();
	mDirectionalLightDeferred = Shader();

    // Destroys the window and quit SDL
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}
