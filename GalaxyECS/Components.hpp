#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <entt/entt.hpp>
#include <vector>
#include <functional>
#include "vec64.hpp"


struct CTransform
{
	vec64 position = vec64(0LL);
	glm::quat rotation = glm::quat();
	glm::vec3 scale = glm::vec3(1.0f);
};

struct CRenderable
{
	std::uint32_t shaderFromSpace = 0;
	std::uint32_t shaderFromAtmosphere = 0;
	bool transparent = false;
	float distance = 0.0f;
};

struct CController
{
	float fov = 0.0f;
	float aspect = 0.0f;
	float zNear = 0.0f;
	float zFar = 0.0f;

	glm::mat4 perspective = glm::mat4(1.0f);
	glm::mat4 PM = glm::mat4(1.0f);
};

struct CSphere
{
	GLuint vao = 0, vbo = 0, vboidx = 0, indices = 0, normalBuffer = 0;

	int patch_max = 64;
	double size_max = 1.0;
	float lodSize = 3.0f;

	float fKr = 0.0025f;
	float fKm = 0.0010f;
	float eSun = 20.0f;
	float rayleighScaleDepth = 0.25f;

	float w1 = 0.650f;
	float w2 = 0.570f;
	float w3 = 0.475f;

	bool renderInside = false;
	bool inAtmosphere = false;
};

struct CGasGiant
{
	std::uint32_t id = 0;
	glm::vec3 colorModifier = glm::vec3(0.0f, 0.0f, 0.0f);
};

struct CPlanet
{
	entt::entity atmosphere = entt::null;
	entt::entity ocean = entt::null;
	glm::vec3 terrainColor1 = glm::vec3(0.0f, 1.0f, 1.0f);
	glm::vec3 terrainColor2 = glm::vec3(0.6f, 0.6f, 0.6f);
	glm::vec3 oceanColor = glm::vec3(0.0f);
	std::uint32_t id = 0;
};

struct CAtmosphere
{
	static entt::entity activeAtmosphere;
	entt::entity planet = entt::null;
	float radius = 1.025f * 1.003f;
};

struct COcean
{
	entt::entity planet = entt::null;
	float radius = 1.006f;
};

struct CSun
{
	glm::vec3 color = glm::vec3(0.0f);
	float time = 0.0f;
};

struct CCorona
{
	GLuint vao = 0;
	GLuint vbo = 0;
};

struct CFlare
{
	GLuint vao = 0;
	GLuint vbo = 0;
};

struct COrbit
{
	entt::entity target = entt::null;
	std::int64_t distance = 0LL;
	float angle = 0.0f;
	float speed = 0.0f;
};

struct CSolarSystem
{
	entt::entity sun = entt::null;
	std::vector<entt::entity> planet;
};

struct CGalaxy
{
	GLuint vao = 0, vbo = 0, colourBuffer = 0;
	std::uint32_t starCount = 0;
	std::uint32_t starCountMax = 0;

	std::vector<glm::vec3> stars;
	std::vector<glm::vec4> colours;
	std::vector<GLfloat> temperatures;
	//std::vector<GLfloat> colours;
	std::vector<double> seeds;
};

struct CNebula
{
	GLuint vao = 0, vbo = 0, colourBuffer = 0;
	std::uint32_t fieldCount = 0;

	std::vector<glm::vec3> fields;
	std::vector<glm::vec4> colours;
};

struct CWindow
{
	bool visible = true;
	std::function<void()> draw = []() {return; };
};