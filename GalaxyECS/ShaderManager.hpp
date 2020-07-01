#pragma once

#include <unordered_map>
#include <array>
#include <cstdlib>
#include <iostream>
#include <string>
#include "Shader.hpp"
#include "Assert.hpp"

class ShaderManager
{
	std::unordered_map<std::uint32_t, std::shared_ptr<Shader>> shaders;

public:
	enum SHADERS: std::uint32_t
	{
		PLANET_FROM_ATMOSPHERE,
		PLANET_FROM_SPACE,
		ATMOSPHERE_FROM_ATMOSPHERE,
		ATMOSPHERE_FROM_SPACE,
		OCEAN_FROM_ATMOSPHERE,
		OCEAN_FROM_SPACE,
		SUN_FROM_ATMOSPHERE,
		SUN_FROM_SPACE,
		GAS_GIANT,
		STAR_CLUSTER,
		CORONA,
		//FLARE,

		BLOOM,
		BLUR,

		NUM_SHADERS
	};

	ShaderManager();
	~ShaderManager();

	void loadShaders();

	std::shared_ptr<Shader> getShader(const std::uint32_t& shader);
	void reloadShaders();
};