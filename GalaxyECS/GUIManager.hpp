#pragma once

#include <entt/entt.hpp>
#include <string>
#include "Components.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "bindings/imgui_impl_glfw.h"
#include "bindings/imgui_impl_opengl3.h"
#include "Globals.hpp"

class GUIManager
{
public:
	entt::entity createSystemWindow(entt::registry& reg);
	entt::entity createGalaxyWindow(entt::registry& reg);
};