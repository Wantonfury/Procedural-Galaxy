#include "ShaderManager.hpp"

ShaderManager::ShaderManager()
{
	
}

ShaderManager::~ShaderManager()
{

}

void ShaderManager::loadShaders()
{
	for (std::uint32_t i = 0; i < NUM_SHADERS; ++i)
	{
		switch (i)
		{
		case PLANET_FROM_ATMOSPHERE:
			shaders.insert({ i, std::make_shared<Shader>("PlanetFromAtmosphere") });
			break;
		case PLANET_FROM_SPACE:
			shaders.insert({ i, std::make_shared<Shader>("PlanetFromSpace") });
			break;
		case ATMOSPHERE_FROM_ATMOSPHERE:
			shaders.insert({ i, std::make_shared<Shader>("AtmosphereFromAtmosphere") });
			break;
		case ATMOSPHERE_FROM_SPACE:
			shaders.insert({ i, std::make_shared<Shader>("AtmosphereFromSpace") });
			break;
		case OCEAN_FROM_ATMOSPHERE:
			shaders.insert({ i, std::make_shared<Shader>("OceanFromAtmosphere") });
			break;
		case OCEAN_FROM_SPACE:
			shaders.insert({ i, std::make_shared<Shader>("OceanFromSpace") });
			break;
		case SUN_FROM_ATMOSPHERE:
			shaders.insert({ i, std::make_shared<Shader>("SunFromAtmosphere") });
			break;
		case SUN_FROM_SPACE:
			shaders.insert({ i, std::make_shared<Shader>("SunFromSpace") });
			break;
		case STAR_CLUSTER:
			shaders.insert({ i, std::make_shared<Shader>("StarCluster") });
			break;
		case BLOOM:
			shaders.insert({ i, std::make_shared<Shader>("bloom") });
			break;
		case BLUR:
			shaders.insert({ i, std::make_shared<Shader>("blur") });
			break;
		case CORONA:
			shaders.insert({ i, std::make_shared<Shader>("corona") });
			break;
		//case FLARE:
			//shaders.insert({ i, std::make_shared<Shader>("Flare") });
			//break;
		case GAS_GIANT:
			shaders.insert({ i, std::make_shared<Shader>("GasGiant") });
			break;
		}
	}
}

std::shared_ptr<Shader> ShaderManager::getShader(const std::uint32_t& shader)
{
	/*assertFunc(shader >= 0 && shader < NUM_SHADERS, [&]() {
		std::cerr << "Requested shader (" + std::to_string(shader) + ") does not exist." << std::endl;
	});*/

	return shaders[shader];
}

void ShaderManager::reloadShaders()
{
	shaders.clear();
	loadShaders();
}