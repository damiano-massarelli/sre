#include "RenderSystem.h"
#include "Shader.h"
#include "Light.h"
#include "Engine.h"
#include "MeshLoader.h"
#include "ShadowMapMaterial.h"
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
    SDL_GL_SetSwapInterval(1);

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        std::terminate();
    }

    initGL(width, height, fovy, nearPlane, farPlane);
	initScreenFbo();
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
    glEnable(GL_BLEND);
	//glEnable(GL_MULTISAMPLE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

void RenderSystem::initScreenFbo()
{
	effectsTarget.create(getScreenWidth(), getScreenHeight());

	MeshLoader loader;
	float verts[]{ -1, -1, -1, 1, 1, 1, 1, -1 };
	float texCoords[]{ 0, 0, 0, 1, 1, 1, 1, 0 };
	std::uint32_t indices[]{ 0, 2, 1, 0, 3, 2 };
	loader.loadData(verts, 8, 2);
	loader.loadData(texCoords, 8, 2);
	loader.loadData(indices, 6, 0, GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_INT);
	mScreenMesh = loader.getMesh(0, 6);
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

void RenderSystem::prepareRendering(const RenderTarget* target)
{
	if (shadowMappingSettings.getShadowStrength() != 0.0f)
		renderShadows();

	std::uint32_t width = getScreenWidth();
	std::uint32_t height = getScreenHeight();
	glEnable(GL_DEPTH_TEST);
	if (target) {
		glBindFramebuffer(GL_FRAMEBUFFER, target->getFbo());
		width = target->getWidth();
		height = target->getHeight();
	}

	glViewport(0, 0, width, height);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Camera calculations */
    glm::mat4 view = glm::mat4{ 1.0f };
	if (camera) 
		view = getViewMatrix(camera->transform);

    /* Sets the camera matrix to a UBO so that it is shared */
	glBindBuffer(GL_UNIFORM_BUFFER, mUboCommonMat);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mProjection));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

    updateLights();
    updateCamera();
}

void RenderSystem::render(RenderPhase phase)
{
	mRenderPhase = phase;
	Engine::gameObjectRenderer.render(Engine::gameObjectManager.getGameObjects());
}

void RenderSystem::finalizeRendering()
{
	if (effectManager.mEnabled) {
		// unbind frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// restore viewport for the screen
		glViewport(0, 0, getScreenWidth(), getScreenHeight());

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		effectManager.mPostProcessingShader.use();
		glBindVertexArray(mScreenMesh.mVao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, effectsTarget.getColorBuffer().getId());
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, effectsTarget.getDepthBuffer().getId());
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)0);
		glEnable(GL_DEPTH_TEST);
	}

    SDL_GL_SwapWindow(mWindow);
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

	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_2D, mShadowMap.getId());
}

void RenderSystem::renderScene(RenderPhase phase, const RenderTarget* target /*= nullptr*/)
{
	/* If effects are enabled and no target is specified
	 * use the effect target */
	auto targetToUse = target;
	if (target == nullptr && effectManager.mEnabled)
		targetToUse = &effectsTarget;

	prepareRendering(targetToUse);
	render(phase);

	if (target == nullptr)
		finalizeRendering();
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

    // Destroys the window and quit SDL
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}
