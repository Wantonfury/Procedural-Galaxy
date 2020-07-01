#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include "ISystem.hpp"
#include "Components.hpp"

class Orbit : public ISystem
{
public:
	Orbit(entt::registry &registry, const std::uint32_t& priority) : ISystem(registry, priority) {};

	void update(float dt) override;
};