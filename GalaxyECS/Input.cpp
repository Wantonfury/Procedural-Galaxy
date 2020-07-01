#include "Input.hpp"

Input* Input::input = NULL;

Input::~Input()
{
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, NULL);
	glfwSetMouseButtonCallback(window, NULL);
	glfwSetCursorPosCallback(window, NULL);
	glfwSetScrollCallback(window, NULL);
}

void Input::setup(GLFWwindow* window)
{
	this->window = window;
	input = this;

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, mouseMoveCallback);
	glfwSetScrollCallback(window, mouseScrollCallback);
}

void Input::mouseScrollCallback(GLFWwindow *window, double xOffset, double yOffset)
{
	if (Globals::Inputs.cursor) return;
	if (yOffset > 0)
		input->speed += input->speed * 0.1f;
	else if (yOffset < 0)
		input->speed -= input->speed * 0.1f;
}

void Input::keyCallback(GLFWwindow *window, int key, int scanCode, int action, int modifiers)
{
	if (key == GLFW_KEY_UNKNOWN) return;

	switch (action)
	{
	case GLFW_RELEASE:
		input->keyPressed[key].first = false;
		input->keyPressed[key].second = false;
		input->keyReleasedPoll.insert(key);
		break;
	case GLFW_PRESS:
		input->keyPressed[key].first = true;
		input->keyPressedPoll.insert(key);
		break;
	case GLFW_REPEAT:
		//input->keyPressed[key].second = true;
		break;
	}
}

void Input::mouseButtonCallback(GLFWwindow *window, int button, int action, int modifiers)
{
	switch (button)
	{
	case GLFW_RELEASE:
		input->onMouseUp(button);
		break;
	case GLFW_PRESS:
		input->onMouseDown(button);
		break;
	}
}

void Input::mouseMoveCallback(GLFWwindow *window, double mousePosX, double mousePosY)
{
	input->mousePosX = mousePosX;
	input->mousePosY = mousePosY;

	if (input->firstTime)
	{
		input->firstTime = false;
		input->mousePosXOld = mousePosX;
		input->mousePosYOld = mousePosY;
	}
}

void Input::onMouseUp(int mouseButton)
{
	switch (mouseButton)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		leftClick = false;
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		rightClick = false;
		break;
	}
}

void Input::onMouseDown(int mouseButton)
{
	switch (mouseButton)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		leftClick = true;
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		rightClick = true;
		break;
	}
}

void Input::update(float dt)
{
	auto view = reg.view<CController, CTransform>();
	if (view.empty()) return;

	CTransform& camera = view.get<CTransform>(view.front());

	float deltaX = (float)(mousePosX - mousePosXOld);
	float deltaY = (float)(mousePosY - mousePosYOld);

	mousePosXOld = mousePosX;
	mousePosYOld = mousePosY;

	glfwMakeContextCurrent(window);

	rotate(camera, dt, deltaX * 0.15f, deltaY * 0.2f);

	for (auto it = keyReleasedPoll.begin(); it != keyReleasedPoll.end(); ++it)
	{
		int key = *it;
		keyPressedPoll.erase(key);

	}
	keyReleasedPoll.clear();

	for (auto it = keyPressedPoll.begin(); it != keyPressedPoll.end(); ++it)
	{
		int key = *it;

		switch (key)
		{
		case GLFW_KEY_W:
			move(camera, dt, Globals::axis[Globals::AXIS_FORWARD], speed);
			break;

		case GLFW_KEY_S:
			move(camera, dt, Globals::axis[Globals::AXIS_BACKWARD], speed);
			break;

		case GLFW_KEY_A:
			move(camera, dt, Globals::axis[Globals::AXIS_LEFT], speed);
			break;

		case GLFW_KEY_D:
			move(camera, dt, Globals::axis[Globals::AXIS_RIGHT], speed);
			break;

		case GLFW_KEY_Q:
			rotateZ(camera, dt, -2.0f);
			break;

		case GLFW_KEY_E:
			rotateZ(camera, dt, 2.0f);
			break;

		case GLFW_KEY_SPACE:
			move(camera, dt, Globals::axis[Globals::AXIS_UP], speed);
			break;

		case GLFW_KEY_C:
			move(camera, dt, Globals::axis[Globals::AXIS_DOWN], speed);
			break;

		case GLFW_KEY_F:
			if (!keyPressed[key].second)
				Globals::Inputs.wireframe = !Globals::Inputs.wireframe;
			break;

		case GLFW_KEY_R:
			if (!keyPressed[key].second)
				Globals::Inputs.hdr = !Globals::Inputs.hdr;
			break;

		case GLFW_KEY_T:
			if (!keyPressed[key].second)
				Globals::Inputs.bloom = !Globals::Inputs.bloom;
			break;

		case GLFW_KEY_LEFT_ALT:
			if (!keyPressed[key].second)
				Globals::Inputs.cursor = !Globals::Inputs.cursor;
			break;

		case GLFW_KEY_TAB:
			if (!keyPressed[key].second)
				Globals::Inputs.gui = !Globals::Inputs.gui;
			break;
		}
		keyPressed[key].second = true;
	}
}

void Input::move(CTransform& camera, float dt, glm::vec3 direction, float speed)
{
	if (Globals::Inputs.cursor) return;
	camera.position = camera.position + (camera.rotation * (direction * speed * dt));
}

void Input::rotate(CTransform& camera, float dt, float horizontal, float vertical)
{
	if (Globals::Inputs.cursor && !leftClick) return;

	camera.rotation = glm::normalize(glm::rotate(camera.rotation, horizontal * dt, Globals::axis[Globals::AXIS_YAW]));
	camera.rotation = glm::normalize(glm::rotate(camera.rotation, vertical * dt, Globals::axis[Globals::AXIS_PITCH]));
}

void Input::rotateZ(CTransform& camera, float dt, float angle)
{
	if (Globals::Inputs.cursor) return;
	camera.rotation = glm::normalize(glm::rotate(camera.rotation, angle * dt, Globals::axis[Globals::AXIS_ROLL]));
}