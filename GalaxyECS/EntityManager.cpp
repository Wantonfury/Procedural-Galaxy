#include "EntityManager.hpp"

entt::entity EntityManager::createPlayer(entt::registry& reg, const vec64& position, float fov, float aspect, float zNear, float zFar)
{
	entt::entity e = reg.create();
	
	reg.assign<CTransform>(e);
	reg.assign<CController>(e, fov, aspect, zNear, zFar);

	auto& transform = reg.get<CTransform>(e);
	transform.position = position;

	return e;
}

entt::entity EntityManager::createOcean(entt::registry& reg, const vec64& position, float scale, entt::entity planet)
{
	entt::entity e = reg.create();

	reg.assign<CRenderable>(e);
	reg.assign<CTransform>(e);
	reg.assign<CSphere>(e);
	reg.assign<COrbit>(e);
	reg.assign<COcean>(e);

	auto& renderable = reg.get<CRenderable>(e);
	auto& sphere = reg.get<CSphere>(e);
	auto& transform = reg.get<CTransform>(e);
	auto& orbit = reg.get<COrbit>(e);
	auto& ocean = reg.get<COcean>(e);

	renderable.shaderFromSpace = Globals::shm->OCEAN_FROM_SPACE;
	renderable.shaderFromAtmosphere = Globals::shm->OCEAN_FROM_ATMOSPHERE;
	renderable.transparent = true;

	transform.position = position;
	transform.scale = glm::vec3(scale * ocean.radius);

	sphere.lodSize = 0.0f;
	//orbit.target = planet;
	//ocean.planet = planet;

	return e;
}

entt::entity EntityManager::createAtmosphere(entt::registry& reg, const vec64& position, float scale, entt::entity planet)
{
	entt::entity e = reg.create();

	reg.assign<CRenderable>(e);
	reg.assign<CTransform>(e);
	reg.assign<CSphere>(e);
	reg.assign<COrbit>(e);
	reg.assign<CAtmosphere>(e);

	auto& renderable = reg.get<CRenderable>(e);
	auto& sphere = reg.get<CSphere>(e);
	auto& transform = reg.get<CTransform>(e);
	auto& orbit = reg.get<COrbit>(e);
	auto& atmosphere = reg.get<CAtmosphere>(e);

	renderable.shaderFromSpace = Globals::shm->ATMOSPHERE_FROM_SPACE;
	renderable.shaderFromAtmosphere = Globals::shm->ATMOSPHERE_FROM_ATMOSPHERE;
	renderable.transparent = true;

	transform.position = position;
	transform.scale = glm::vec3(scale * atmosphere.radius);

	//orbit.target = planet;
	//atmosphere.planet = planet;
	sphere.renderInside = true;
	sphere.lodSize = 0.0f;


	return e;
}
static int test = 0;
entt::entity EntityManager::createPlanet(entt::registry& reg, const vec64& position, float scale, entt::entity sun, std::int64_t distance, float angle, bool atmosphere, bool ocean)
{
	entt::entity eAtmo, eOcean;

	if (atmosphere) eAtmo = createAtmosphere(reg, position, scale, entt::null);
	if (ocean) eOcean = createOcean(reg, position, scale, entt::null);

	entt::entity e = reg.create();

	reg.assign<CRenderable>(e);
	reg.assign<CTransform>(e);
	reg.assign<CSphere>(e);
	reg.assign<CPlanet>(e);
	reg.assign<COrbit>(e);

	auto& renderable = reg.get<CRenderable>(e);
	auto& sphere = reg.get<CSphere>(e);
	auto& transform = reg.get<CTransform>(e);
	auto& orbit = reg.get<COrbit>(e);
	auto& planet = reg.get<CPlanet>(e);

	renderable.shaderFromSpace = Globals::shm->PLANET_FROM_SPACE;
	renderable.shaderFromAtmosphere = Globals::shm->PLANET_FROM_ATMOSPHERE;

	transform.position = position;
	transform.scale = glm::vec3(scale);

	orbit.target = sun;
	orbit.angle = angle;
	orbit.distance = distance;

	planet.id = planets++;
	
	reg.get<CAtmosphere>(eAtmo).planet = e;
	reg.get<COrbit>(eAtmo).target = e;
	reg.get<COcean>(eOcean).planet = e;
	reg.get<COrbit>(eOcean).target = e;

	return e;
}

entt::entity EntityManager::createSun(entt::registry& reg, const vec64& position, float scale)
{
	entt::entity e = reg.create();

	reg.assign<CRenderable>(e);
	reg.assign<CTransform>(e);
	reg.assign<CSphere>(e);
	reg.assign<CSun>(e);

	auto& renderable = reg.get<CRenderable>(e);
	auto& sphere = reg.get<CSphere>(e);
	auto& transform = reg.get<CTransform>(e);
	auto& sun = reg.get<CSun>(e);

	renderable.shaderFromSpace = Globals::shm->SUN_FROM_SPACE;
	renderable.shaderFromAtmosphere = Globals::shm->SUN_FROM_SPACE;//Globals::shm->SUN_FROM_ATMOSPHERE;

	transform.position = position;
	transform.scale = glm::vec3(scale);

	sphere.lodSize = 4.0f;
	sphere.patch_max = 64;

	sun.color = glm::vec3(2.0f, 0.8f, 0.1f);

	createCorona(reg, position, scale, e);
	createFlare(reg, position, scale, e);

	return e;
}

entt::entity EntityManager::createGasGiant(entt::registry& reg, const vec64& position, float scale, entt::entity sun, std::int64_t distance, float angle)
{
	entt::entity e = reg.create();

	reg.assign<CRenderable>(e);
	reg.assign<CTransform>(e);
	reg.assign<CSphere>(e);
	reg.assign<CGasGiant>(e);
	reg.assign<COrbit>(e);

	auto& renderable = reg.get<CRenderable>(e);
	auto& sphere = reg.get<CSphere>(e);
	auto& transform = reg.get<CTransform>(e);
	auto& gas = reg.get<CGasGiant>(e);
	auto& orbit = reg.get<COrbit>(e);

	renderable.shaderFromSpace = Globals::shm->GAS_GIANT;
	renderable.shaderFromAtmosphere = Globals::shm->GAS_GIANT;

	transform.position = position;
	transform.scale = glm::vec3(scale);

	sphere.lodSize = 4.0f;
	sphere.patch_max = 64;

	orbit.target = sun;
	orbit.angle = angle;
	orbit.distance = distance;

	gas.id = gasGiants++;

	return e;
}

entt::entity EntityManager::createCorona(entt::registry& reg, const vec64& position, float scale, entt::entity sun)
{
	entt::entity e = reg.create();
	reg.assign<CTransform>(e);
	reg.assign<CRenderable>(e);
	reg.assign<CCorona>(e);
	reg.assign<COrbit>(e);

	auto& orbit = reg.get<COrbit>(e);
	orbit.target = sun;

	return e;
}

entt::entity EntityManager::createFlare(entt::registry& reg, const vec64& position, float scale, entt::entity sun)
{
	entt::entity e = reg.create();
	reg.assign<CTransform>(e);
	reg.assign<CRenderable>(e);
	reg.assign<CFlare>(e);
	reg.assign<COrbit>(e);

	auto& orbit = reg.get<COrbit>(e);
	orbit.target = sun;

	return e;
}

entt::entity EntityManager::createSolarSystem(entt::registry& reg, const vec64& position)
{
	entt::entity e = reg.create();

	// TODO

	return e;
}

entt::entity EntityManager::createStarCluster(entt::registry& reg, const vec64& position, const std::uint32_t& starCount)
{
	entt::entity e = reg.create();

	reg.assign<CRenderable>(e);
	reg.assign<CTransform>(e);
	reg.assign<CGalaxy>(e);

	auto& renderable = reg.get<CRenderable>(e);
	auto& transform = reg.get<CTransform>(e);
	auto& galaxy = reg.get<CGalaxy>(e);

	renderable.shaderFromSpace = Globals::shm->STAR_CLUSTER;
	renderable.shaderFromAtmosphere = Globals::shm->STAR_CLUSTER;

	transform.position = position;
	galaxy.starCount = starCount;

	return e;
}

void EntityManager::createGalaxy(entt::registry& reg, const vec64& position, const std::uint32_t& arms, const std::uint32_t& armLength, const std::uint32_t& coreStarCount)
{
	const float distanceMax = 8137418400.500f;
	float distance = 8137418400.500f;
	std::uint32_t starCount = coreStarCount * 0.1;

	// Create core
	auto e = createStarCluster(reg, position, coreStarCount);
	//reg.get<CTransform>(e).scale = glm::vec3(40.0f);

	// Create arms
	for (std::uint32_t i = 0; i < arms; ++i)
	{
		float angle = (2.0f * 3.141592653f) * ((float)i / (float)arms);
		std::uint32_t stars = starCount;
		
		for (std::uint32_t j = armLength; j > 0; --j)
		{
			stars *= (float)j / (float)armLength;
			distance = distanceMax - distanceMax * ((1.0f - (float)j / (float)armLength) * 0.7f);

			angle += 0.175f; // 10 degrees
			//angle += 0.0175f;
			//if (j > armLength / 2) angle -= 0.0872f; // 5 degrees //0.0175f; // 1 degree

			vec64 starPos = vec64(position.getX() - cos(angle) * (distance * (float)(armLength - j)), position.getY(), position.getZ() - sin(angle) * (distance * (float)(armLength - j)));

			createStarCluster(reg, starPos, stars);
		}
		std::cout << std::endl;
	}
}

entt::entity EntityManager::createGalaxyEXP(entt::registry& reg, const vec64& position, const std::uint32_t& arms, const std::uint32_t& armLength, const std::uint32_t& starCount, const double& seed)
{
	/*const float distanceMax = 8137418400.500f;
	float distance = 8137418400.500f;
	std::uint32_t starCount = coreStarCount * 0.1;

	// Create core
	auto e = createStarCluster(reg, position, coreStarCount);
	reg.get<CTransform>(e).scale = glm::vec3(200.0f);*/

	const float distanceMax = 200.0f;//5137418400.500f;
	float distance = 200.0f;//5137418400.500f;
	std::mt19937 rand(seed);
	std::normal_distribution<float> colour(0.0f, 1.0f);
	std::uniform_int_distribution<GLint> temperature(0, 0);

	std::normal_distribution<float> pos(0.0f, 200.0f);
	std::normal_distribution<float> height(0.0f, 500.0f);

	std::uniform_real_distribution<float> starClass(0.0f, 100.0f);
	std::uniform_real_distribution<double> starSeed(-9999999.9, 99999999.9);

	entt::entity e = reg.create();

	reg.assign<CRenderable>(e);
	reg.assign<CTransform>(e);
	reg.assign<CGalaxy>(e);

	auto& renderable = reg.get<CRenderable>(e);
	auto& transform = reg.get<CTransform>(e);
	auto& galaxy = reg.get<CGalaxy>(e);

	renderable.shaderFromSpace = Globals::shm->STAR_CLUSTER;
	renderable.shaderFromAtmosphere = Globals::shm->STAR_CLUSTER;

	transform.position = position;
	float scale = 900000000.0f;
	//float scale = 10000.0f;
	galaxy.starCount = starCount;

	galaxy.stars.reserve(galaxy.starCount);
	galaxy.colours.reserve(galaxy.starCount);
	galaxy.seeds.reserve(galaxy.starCount);
	galaxy.temperatures.reserve(galaxy.starCount);

	std::uint32_t starCountTotal = 0;
	// Create arms
	for (std::uint32_t i = 0; i < arms; ++i)
	{
		float angleArm = (2.0f * 3.141592653f) * ((float)i / (float)arms);
		std::uint32_t stars = starCount / arms;
		float offset = 10.0f;

		for (std::uint32_t j = armLength; j > 0; --j)
		{
			//stars *= (float)j / (float)armLength;
			//stars = 4;
			//distance = distanceMax - distanceMax * ((1.0f - (float)j / (float)armLength) * 0.7f);
			//distance = distanceMax + distanceBegin;
			//distanceBegin = 0.0f;
			distance = distanceMax + offset;

			//angle += distance * 0.075f; // 10 degrees
			//v *= Matrix4.CreateRotationY(-v.Magnitude * rotationStrength);
			//angle += distance * 0.0000001f;
			//glm::vec3 v = position.getVec3f() * distance;
			//v *= glm::quat(0.0f, 0.0f, -glm::length(v) * 0.0001f, 0.0f);
			//angle = 0.0f;
			//v *= glm::rotate(glm::mat4(1.0f), -glm::length(v) * 50.0f, glm::vec3(0.0f, 1.0f, 0.0f));

			for (std::uint32_t star = 0; star < stars; ++star)
			{
				

				float s = starClass(rand);
				if (s <= 76.45f)
				{
					temperature = std::uniform_int_distribution<GLint>(800, 3700);
				}
				else if (s <= 88.55)
				{
					temperature = std::uniform_int_distribution<GLint>(3701, 5200);
				}
				else if (s <= 96.15)
				{
					temperature = std::uniform_int_distribution<GLint>(5201, 6000);
				}
				else if (s <= 99.15)
				{
					temperature = std::uniform_int_distribution<GLint>(6001, 7500);
				}
				else if (s <= 99.75)
				{
					temperature = std::uniform_int_distribution<GLint>(7501, 10000);
				}
				else if (s <= 99.88)
				{
					temperature = std::uniform_int_distribution<GLint>(10001, 30000);
				}
				else
				{
					temperature = std::uniform_int_distribution<GLint>(30000, 30000);
				}

				GLfloat kelvin = (GLfloat)temperature(rand);
				glm::vec4 colour = glm::vec4(kelvin * (0.0534 / 255.0) - (43.0 / 255.0), kelvin * (0.0628 / 255.0) - (77.0 / 255.0), kelvin * (0.0735 / 255.0) - (115.0 / 255.0), 1.0f);

				glm::vec3 starPos = glm::vec3(pos(rand) - cos(angleArm) * (distance * (float)(armLength - j)), height(rand) / (float)(armLength + 1 - j), pos(rand) - sin(angleArm) * (distance * (float)(armLength - j)));
				float angle = -glm::length(starPos) * 0.1;//0.000000004;//-glm::length(pos) * 0.2f;
				auto rot = glm::angleAxis(glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

				starPos = starPos * rot;
				starPos *= scale;
				

				galaxy.stars.push_back(starPos);
				galaxy.seeds.push_back(starSeed(rand));
				galaxy.temperatures.push_back(kelvin);
				galaxy.colours.push_back(colour);
				++starCountTotal;
			}

			//glm::vec3 starPos = glm::vec3(position.getX() - cos(angle) * (distance * (float)(armLength - j)), position.getY(), position.getZ() - sin(angle) * (distance * (float)(armLength - j)));
			//cluster.stars.push_back(starPos);
			//++starCountTotal;
		}
	}

	/*for (std::uint32_t i = 0; i < starCountTotal; ++i)
	{
		glm::vec3& pos = cluster.stars[i];

		float angle = -glm::length(pos) * 0.2;//0.000000004;//-glm::length(pos) * 0.2f;
		auto rot = glm::angleAxis(glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 rotMatrix = glm::mat4_cast(rot);
		
		pos *= scale;
		pos = pos * rot;
	}*/

	galaxy.starCount = starCountTotal;
	galaxy.starCountMax = starCountTotal;
	std::cout << "starCount: " << starCount << std::endl << "starCounTotal: " << starCountTotal << std::endl;

	//createNebula(reg, position, galaxy.starCount, e);

	return e;
}