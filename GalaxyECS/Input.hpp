#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <set>
#include <utility>
#include "vec64.hpp"
#include "ISystem.hpp"
#include "Globals.hpp"
#include "Components.hpp"

class Input : public ISystem
{
	GLFWwindow *window;

	double mousePosXOld = 0.0;
	double mousePosYOld = 0.0;
	double mousePosX = 0.0;
	double mousePosY = 0.0;

	float speed = 10000000.0f;

	bool firstTime = true;
	bool leftClick = false;
	bool rightClick = false;

	std::pair<bool, bool> keyPressed[GLFW_KEY_MENU + 1]; // last glfw key macro + 1 // first = key pressed, second = key is repeating
	std::set<int> keyReleasedPoll;
	std::set<int> keyPressedPoll;

	static Input *input;

	static void keyCallback(GLFWwindow *window, int key, int scanCode, int action, int modifiers);
	static void mouseButtonCallback(GLFWwindow *window, int button, int action, int modifiers);
	static void mouseMoveCallback(GLFWwindow *window, double mousePosX, double mousePosY);
	static void mouseScrollCallback(GLFWwindow *window, double xOffset, double yOffset);

	void onMouseUp(int mouseButton);
	void onMouseDown(int mouseButton);

	void move(CTransform& camera, float dt, glm::vec3 direction, float speed);
	void rotate(CTransform& camera, float dt, float horizontal, float vertical);
	void rotateZ(CTransform& camera, float dt, float angle);

public:
	Input(entt::registry& registry, const std::uint32_t& priority) : ISystem(registry, priority) {};
	~Input();

	void update(float dt) override;

	void setup(GLFWwindow* window);
};