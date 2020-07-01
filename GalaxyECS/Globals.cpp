#include "Globals.hpp"

namespace Globals
{
	std::unique_ptr<ShaderManager> shm = std::make_unique<ShaderManager>();

	InputStruct Inputs;
	glm::vec3 axis[AXIS_NUM];

	const float atmosphereRadius = 1.025f;
	const float oceanRadius = 0.98f;

	void initGlobals()
	{
		axis[AXIS_YAW].y = -1;
		axis[AXIS_YAW_REVERSED].y = -axis[AXIS_YAW].y; // z

		axis[AXIS_PITCH].x = -1;
		axis[AXIS_PITCH_REVERSED].x = -axis[AXIS_PITCH].x; // z

		axis[AXIS_ROLL].z = -1;
		axis[AXIS_ROLL_REVERSED].z = -axis[AXIS_ROLL].z;

		axis[AXIS_FORWARD].z = 1;
		axis[AXIS_BACKWARD].z = -axis[AXIS_FORWARD].z;

		axis[AXIS_LEFT].x = 1;
		axis[AXIS_RIGHT].x = -axis[AXIS_LEFT].x;

		axis[AXIS_UP].y = -1;
		axis[AXIS_DOWN].y = -axis[AXIS_UP].y;

		Inputs.hdr = true;
		Inputs.bloom = true;
		Inputs.wireframe = false;
		Inputs.cursor = false;
		Inputs.gui = true;
	}
}