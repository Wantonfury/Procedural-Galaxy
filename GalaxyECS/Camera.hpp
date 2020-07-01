#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include "ISystem.hpp"
#include "Components.hpp"

class Camera : public ISystem
{
public:
	Camera(entt::registry& registry, const std::uint32_t& priority) : ISystem(registry, priority) {};

	void update(float dt) override;
	void setup(float fov, float aspect, float zNear, float zFar);
};