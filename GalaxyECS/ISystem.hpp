#pragma once

#include <entt/entt.hpp>
#include <iostream>

class ISystem
{
protected:
	std::uint32_t priority;
	entt::registry& reg;

	ISystem(entt::registry& registry, const std::uint32_t& priority) : reg(registry), priority(priority) {};
	~ISystem() {}
	virtual void init() {};

public:
	void setPriority(const std::uint32_t& priority);
	std::uint32_t getPriority();

	virtual void update(float dt) = 0;
};