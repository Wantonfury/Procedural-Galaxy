#pragma once

#include <memory>
#include "ShaderManager.hpp"

namespace Globals
{
	enum {
		AXIS_YAW,
		AXIS_PITCH,
		AXIS_ROLL,
		AXIS_YAW_REVERSED,
		AXIS_PITCH_REVERSED,
		AXIS_ROLL_REVERSED,
		AXIS_FORWARD,
		AXIS_BACKWARD,
		AXIS_LEFT,
		AXIS_RIGHT,
		AXIS_UP,
		AXIS_DOWN,

		AXIS_NUM
	};

	struct InputStruct
	{
		bool wireframe;
		bool hdr;
		bool bloom;
		bool cursor;
		bool gui;
	};

	extern std::unique_ptr<ShaderManager> shm;

	extern InputStruct Inputs;
	extern glm::vec3 axis[];

	extern const float atmosphereRadius;
	extern const float oceanRadius;

	void initGlobals();
}