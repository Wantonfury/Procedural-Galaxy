#include <iostream>
#include <entt/entt.hpp>
#include <memory>
#include "Globals.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "Input.hpp"
#include "Orbit.h"
#include "Galaxy.hpp"
#include "SystemManager.hpp"
#include "ShaderManager.hpp"
#include "EntityManager.hpp"
#include "GUIManager.hpp"
#include "Assert.hpp"

int main()
{
	float dt = 0.0f;
	entt::registry reg;

	Globals::initGlobals();
	SystemManager sm;

	// Setup systems
	std::uint32_t priority = 0;
	std::shared_ptr<Renderer> renderer = sm.registerSystem<Renderer>(reg, priority++);
	std::shared_ptr<Orbit> orbit = sm.registerSystem<Orbit>(reg, priority++);
	std::shared_ptr<Galaxy> galaxy = sm.registerSystem<Galaxy>(reg, priority++);
	std::shared_ptr<Camera> camera = sm.registerSystem<Camera>(reg, priority++);
	std::shared_ptr<Input> input = sm.registerSystem<Input>(reg, priority++);

	Globals::shm->loadShaders();
	renderer->setupShaders();
	input->setup(renderer->getWindow());

	// Setup player and camera
	EntityManager em;
	entt::entity cameraEntity = em.createPlayer(reg, vec64(0LL), 70.0f, (float)renderer->getWidth() / (float)renderer->getHeight(), 1000.0f, 2147483647.0f);
	CController& cameraData = reg.get<CController>(cameraEntity);
	auto& cameraTransform = reg.get<CTransform>(cameraEntity);

	entt::entity sun = em.createSun(reg, vec64(40000000000000LL, 0LL, 0LL), 1000000.0f);
	entt::entity planet = em.createPlanet(reg, vec64(0LL, 0LL, 0LL), 400000.0f, sun, 40000000000000LL, glm::radians(0.0f), true, true);
	planet = em.createPlanet(reg, vec64(0LL, 0LL, 0LL), 400000.0f, sun, 40000000000000LL, glm::radians(20.0f), true, true);
	planet = em.createPlanet(reg, vec64(0LL, 0LL, 0LL), 400000.0f, sun, 40000000000000LL, glm::radians(40.0f), true, true);
	planet = em.createPlanet(reg, vec64(0LL, 0LL, 0LL), 400000.0f, sun, 40000000000000LL, glm::radians(60.0f), true, true);
	planet = em.createPlanet(reg, vec64(0LL, 0LL, 0LL), 400000.0f, sun, 40000000000000LL, glm::radians(80.0f), true, true);
	planet = em.createPlanet(reg, vec64(0LL, 0LL, 0LL), 400000.0f, sun, 40000000000000LL, glm::radians(100.0f), true, true);
	planet = em.createPlanet(reg, vec64(0LL, 0LL, 0LL), 400000.0f, sun, 40000000000000LL, glm::radians(120.0f), true, true);
	planet = em.createPlanet(reg, vec64(0LL, 0LL, 0LL), 400000.0f, sun, 40000000000000LL, glm::radians(140.0f), true, true);
	planet = em.createPlanet(reg, vec64(0LL, 0LL, 0LL), 400000.0f, sun, 40000000000000LL, glm::radians(160.0f), true, true);
	//planet = em.createGasGiant(reg, vec64(0LL, 0LL, 0LL), 400000.0f, sun, 40000000000000LL, glm::radians(180.0f));

	entt::entity galaxyEntity = em.createGalaxyEXP(reg, vec64(0LL), 4, 13, 80000, 0.0);
	auto& galaxyComp = reg.get<CGalaxy>(galaxyEntity);
	reg.get<CTransform>(sun).position = vec64(-galaxyComp.stars[0].x, -galaxyComp.stars[0].y, -galaxyComp.stars[0].z);
	cameraTransform.position = vec64(-galaxyComp.stars[galaxyComp.starCountMax].x, -galaxyComp.stars[galaxyComp.starCountMax].y, -galaxyComp.stars[galaxyComp.starCountMax].z);

	GUIManager gui;
	entt::entity window = gui.createSystemWindow(reg);
	window = gui.createGalaxyWindow(reg);

	while (!renderer->shouldClose())
	{
		// Setup deltaTime
		renderer->updateDeltaTime(dt);

		sm.update(dt);
	}
}