#include "Galaxy.hpp"

void Galaxy::updateStars(int oldId, int starId, CGalaxy& galaxy)
{
	glBindBuffer(GL_ARRAY_BUFFER, galaxy.colourBuffer);

	if (oldId >= 0) glBufferSubData(GL_ARRAY_BUFFER, oldId * sizeof(glm::vec4), sizeof(glm::vec4), &galaxy.colours[oldId]);
	if (starId >= 0) glBufferSubData(GL_ARRAY_BUFFER, starId * sizeof(glm::vec4), sizeof(glm::vec4), &galaxy.colours[starId]);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Galaxy::update(float dt)
{
	auto suns = reg.view<CSun>();
	if (suns.empty()) return;
	auto cameras = reg.view<CController>();
	if (cameras.empty()) return;

	auto& sunTransform = reg.get<CTransform>(suns.front());
	auto& sun = reg.get<CSun>(suns.front());
	auto& cameraTransform = reg.get<CTransform>(cameras.front());
	auto view = reg.view<CGalaxy, CTransform>();

	glm::vec3 cameraPos = cameraTransform.position.getVec3f();
	Point cameraPoint = Point(cameraPos.x, cameraPos.y, cameraPos.z);
	float distanceMin = 1000000000.0f;

	for (const auto& entity : view)
	{
		auto& galaxy = view.get<CGalaxy>(entity);
		auto& transform = view.get<CTransform>(entity);
		

		if (points.empty() && galaxy.vao)
		{
			points.reserve(galaxy.starCount);
			for (std::uint32_t i = 0; i < galaxy.starCount; ++i)
			{
				points.push_back(Point(-galaxy.stars[i].x, -galaxy.stars[i].y, -galaxy.stars[i].z));
			}
			kdtree.build(points);
		}
		if (!points.empty())
		{
			auto star = kdtree.nnSearch(cameraPoint);
			float distance = glm::distance(cameraPos, -galaxy.stars[star]);

			if (starCurrent != star && distance <= distanceMin)
			{
				galaxy.colours[star].w = 0.0f;
				if (starCurrent >= 0) galaxy.colours[starCurrent].w = 1.0f;

				sunTransform.position = vec64(-galaxy.stars[star].x, -galaxy.stars[star].y, -galaxy.stars[star].z);
				sun.color = glm::vec3(galaxy.colours[star].x, galaxy.colours[star].y, galaxy.colours[star].z);

				generateSystem(star, galaxy);
				updateStars(starCurrent, star, galaxy);

				starCurrent = star;
			}

			if (starCurrent >= 0)
			{
				if (galaxy.colours[starCurrent].w == 1.0f && distance <= distanceMin)
				{
					galaxy.colours[starCurrent].w = 0.0f;
					updateStars(starCurrent, -1, galaxy);
				}
				if (galaxy.colours[starCurrent].w == 0.0f && distance > distanceMin)
				{
					galaxy.colours[starCurrent].w = 1.0f;
					updateStars(starCurrent, -1, galaxy);
					starCurrent = -1;
				}
			}
		}
	}
}

void Galaxy::generateSystem(int star, CGalaxy& galaxy)
{
	std::mt19937 rand(galaxy.seeds[star]);
	std::uniform_real_distribution<float> color(-0.5f, 0.5f);
	std::uniform_real_distribution<float> randomDistance(10000000000000.0f, 30000000000000.0f);
	std::uniform_real_distribution<float> randomAngle(10.0f, 20.0f);

	float distance = 30000000000000.0f + randomDistance(rand);
	float angle = glm::radians(-10.0f) + glm::radians(randomAngle(rand));

	auto view = reg.view<CPlanet, COrbit>();
	for (const auto& entity : view)
	{
		auto& planet = view.get<CPlanet>(entity);
		auto& orbit = view.get<COrbit>(entity);

		planet.terrainColor1 = glm::vec3(1.0f - color(rand), 1.0f - color(rand), 1.0f - color(rand));
		planet.terrainColor2 = glm::vec3(0.0f - color(rand), 1.0f - color(rand), 0.0f - color(rand));
		planet.oceanColor = glm::vec3(0.25f - color(rand), 0.25f - color(rand), 1.0f - color(rand));
		
		orbit.distance = distance;
		orbit.angle = angle;

		distance += randomDistance(rand);
		angle += randomAngle(rand);
	}
}