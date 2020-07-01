#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "bindings/imgui_impl_glfw.h"
#include "bindings/imgui_impl_opengl3.h"
#include <entt/entt.hpp>
#include <libpng16/png.h>
#include <string>
#include <cmath>
#include <random>
#include <functional>
#include <iostream>
#include "ISystem.hpp"
#include "Components.hpp"
#include "Globals.hpp"
#include "Assert.hpp"

class Renderer : public ISystem
{
	struct Texture
	{
		std::string fileName;
		int width, height;
		bool alpha;
		GLubyte *data = NULL;
		GLuint ID = 0;
	};

	enum TEXTURE: std::uint32_t
	{
		TEXTURE_STAR_SPECTRUM,
		TEXTURE_STAR_FLARE,
		TEXTURE_GAS_GIANT,

		NUM_TEXTURES
	};

	const std::string TEXTURE_PATH = "textures/";

	std::unordered_map<std::uint32_t, std::shared_ptr<Texture>> texturePool;
	const static std::unordered_map<TEXTURE, const char*> textureNames;
	const static GLfloat coronaData[];

	float timeLast = 0.0f;
	float timeCurrent = 0.0f;
	float exposure = 2.2f;

	float heightOfNearPlane = 0.0f;
	const float spaceNear = 1000.0f; //1000.0f; //1000.0f;
	const float spaceFar = 214748369.0f; //4147483647.0f; //2147483647.0f;
	const float planetNear = 0.1f;
	const float planetFar = 214748369.0f;

	int width = 1920;
	int height = 1080;
	int tris_rendered = 0;

	const std::string TITLE = "Space";

	GLuint hdrFBO, colourBuffers[2], rboDepth, pingpongFBO[2], attachments[2], pingpongColourBuffers[2];
	GLuint quadVAO = 0, quadVBO = 0;

	GLFWwindow *window = NULL;
	std::shared_ptr<Shader> shaderBlur = nullptr;
	std::shared_ptr<Shader> shaderBloom = nullptr;
	std::shared_ptr<Shader> coronaShader = nullptr;
	std::shared_ptr<Shader> flareShader = nullptr;

	void init() override;
	static void glfwErrorCallback(int error, const char* description);
	void renderScreenQuad();

	glm::mat4 getModelViewMatrix(CTransform& transform);
	bool loadPngImage(const char* name, int& width, int& height, bool& alpha, GLubyte** data);

	void renderGUI(float dt);
	void renderStars(float dt, CTransform& cameraTransform, CController& cameraData);
	void renderSpheres(float dt, CTransform& cameraTransform, CController& cameraData);
	void renderSphere(float dt, CTransform& cameraTransform, CController& cameraData, CTransform& sunTransform, CSun& sun, entt::entity entity, bool& nearClip);
	void renderCoronas(float dt, CTransform& cameraTransform, CController& cameraData);
	void renderFlares(float dt, CTransform& cameraTransform, CController& cameraData);

	// Utilities
	void generateStars(CGalaxy& cluster);
	void generateCorona(CCorona& corona);
	void generateFlare(CFlare& flare);
	void loadTextures();

	// Sphere drawing
	void patchBegin(CSphere& sphere, std::shared_ptr<Shader>& shader);
	void patchEnd(CSphere& sphere, std::shared_ptr<Shader>& shader);

	void draw(CSphere& sphere, std::shared_ptr<Shader>& shader, glm::dvec3 center, glm::mat4& VPM);
	void drawRecursive(CSphere& sphere, std::shared_ptr<Shader>& shader, glm::dvec3 p1, glm::dvec3 p2, glm::dvec3 p3, glm::dvec2 uv1, glm::dvec2 uv2, glm::dvec2 uv3, glm::dvec3 center, glm::mat4& VPM, double size = 1.0);
	void drawPatch(CSphere& sphere, std::shared_ptr<Shader>& shader, glm::dvec3 p1, glm::dvec3 p2, glm::dvec3 p3, glm::dvec2 uv1, glm::dvec2 uv2, glm::dvec2 uv3, glm::dvec3 center, glm::mat4& VPM, double size = 1.0);

public:
	Renderer(entt::registry& registry, const std::uint32_t& priority) : ISystem(registry, priority) { init(); };

	void update(float dt) override;
	void updateDeltaTime(float &dt);

	void setupShaders();

	GLFWwindow* getWindow();
	int getWidth();
	int getHeight();
	bool shouldClose();
};