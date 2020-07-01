#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <random>
#include "Globals.hpp"
#include "Components.hpp"
#include "vec64.hpp"

class EntityManager
{
	std::uint32_t planets = 0;
	std::uint32_t gasGiants = 0;

public:
	entt::entity createGalaxyEXP(entt::registry& reg, const vec64& position, const std::uint32_t& arms, const std::uint32_t& armLength, const std::uint32_t& starCount, const double& seed);
	void createGalaxy(entt::registry& reg, const vec64& position, const std::uint32_t& arms, const std::uint32_t& armLength, const std::uint32_t& coreStarCount);
	entt::entity createStarCluster(entt::registry& reg, const vec64& position, const std::uint32_t& starCount);

	entt::entity createPlayer(entt::registry& reg, const vec64& position, float fov, float aspect, float zNear, float zFar);
	entt::entity createSolarSystem(entt::registry& reg, const vec64& position = vec64(0LL));
	entt::entity createSun(entt::registry& reg, const vec64& position, float scale);
	entt::entity createCorona(entt::registry& reg, const vec64& position, float scal, entt::entity sun);
	entt::entity createFlare(entt::registry& reg, const vec64& position, float scal, entt::entity sun);
	entt::entity createPlanet(entt::registry& reg, const vec64& position, float scale, entt::entity sun, std::int64_t distance, float angle, bool atmoshere = false, bool ocean = false);
	entt::entity createAtmosphere(entt::registry& reg, const vec64& position, float scale, entt::entity planet);
	entt::entity createOcean(entt::registry& reg, const vec64& position, float scale, entt::entity planet);
	entt::entity createGasGiant(entt::registry& reg, const vec64& position, float scale, entt::entity sun, std::int64_t distance, float angle);
};