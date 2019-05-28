#include "RenderSystem.h"
#include "Shader.h"
#include "Light.h"
#include "Engine.h"
#include "MeshLoader.h"
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

void RenderSystem::createWindow(std::uint32_t width, std::uint32_t height, float fovy, float nearPlane, float farPlane, std::uint32_t samples)
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

	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, samples);

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
	initScreenFbo();
}

void RenderSystem::initGL(std::uint32_t width, std::uint32_t height, float fovy, float nearPlane, float farPlane)
{
    mProjection = glm::perspective(fovy, static_cast<float>(width)/height, nearPlane, farPlane);

    /* Uniform buffer object set up for common matrices */
    glGenBuffers(1, &mUboCommonMat);
    glBindBuffer(GL_UNIFORM_BUFFER, mUboCommonMat);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
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
	mColorBuffer = Texture::load(nullptr, 1280, 720, GL_REPEAT, GL_REPEAT, false, GL_RGB);
	mDepthBuffer = Texture::load(nullptr, 1280, 720, GL_REPEAT, GL_REPEAT, false, GL_DEPTH_COMPONENT, GL_FLOAT);

	glGenFramebuffers(1, &mScreenFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, mScreenFbo);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorBuffer.getId(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthBuffer.getId(), 0);

	/** Render buffer objects can be used instead of textures, they are faster but read only.
	  * use textures when you need to read from them, use rbo when you dont */
// 	std::uint32_t rbo;
// 	glGenRenderbuffers(1, &rbo);
// 	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
// 	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1280, 720);
// 	glBindRenderbuffer(GL_RENDERBUFFER, 0);
/*	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);*/

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Frame buffer is incomplete";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	MeshLoader loader;
	float verts[]{ -1, -1, -1, 1, 1, 1, 1, -1 };
	float texCoords[]{ 0, 0, 0, 1, 1, 1, 1, 0 };
	std::uint32_t indices[]{ 0, 2, 1, 0, 3, 2 };
	loader.loadData(verts, 8, 2);
	loader.loadData(texCoords, 8, 2);
	loader.loadData(indices, 6, 0, GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_INT);
	mScreenMesh = loader.getMesh(0, 6);
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

void RenderSystem::prepareRendering()
{
	glEnable(GL_DEPTH_TEST);
	if (effectManager.mEnabled)
		glBindFramebuffer(GL_FRAMEBUFFER, mScreenFbo);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Camera calculations */
    glm::mat4 view = glm::mat4{1.0f};
    if (camera) {
        view = glm::translate(glm::mat4{1.0f}, -camera->transform.getPosition());
        glm::mat4 inverseRotation = glm::transpose(glm::toMat4(camera->transform.getRotation()));
        view = inverseRotation * view;
    }

    /* Inverts the view so that we watch in the direction of camera->transform.forward() */
    glm::mat4 invert{
        glm::vec4{-1, 0, 0, 0},
        glm::vec4{0, 1, 0, 0},
        glm::vec4{0, 0, -1, 0},
        glm::vec4{0, 0, 0, 1}
    };
    view = invert * view;


    /* Sets the camera matrix to a UBO so that it is shared */
    glBindBuffer(GL_UNIFORM_BUFFER, mUboCommonMat);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    updateLights();
    updateCamera();
}

void RenderSystem::finalizeRendering()
{
	if (effectManager.mEnabled) {
		// unbind frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		effectManager.mPostProcessingShader;
		glBindVertexArray(mScreenMesh.mVao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mColorBuffer.getId());
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)0);
	}

    SDL_GL_SwapWindow(mWindow);
}

void RenderSystem::addLight(const GameObjectEH& light)
{
    if (light->getComponent<Light>() == nullptr) {
        std::cerr << "Adding a light without a Light component, discarded\n";
        return;
    }

    mLights.push_back(light);
}

void RenderSystem::cleanUp()
{
    // Delete uniform buffers
    glDeleteBuffers(1, &mUboCommonMat);
    glDeleteBuffers(1, &mUboLights);
	glDeleteFramebuffers(1, &mScreenFbo);

	effectManager.cleanUp();

    // Destroys the window and quit SDL
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}
