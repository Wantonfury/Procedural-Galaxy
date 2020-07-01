#include "GUIManager.hpp"

entt::entity GUIManager::createSystemWindow(entt::registry& reg)
{
	entt::entity e = reg.create();

	reg.assign<CRenderable>(e);
	reg.assign<CTransform>(e);
	reg.assign<CWindow>(e);

	auto& window = reg.get<CWindow>(e);
	window.draw = [&reg]() {
		ImGui::Begin("Planet Position/Color");

		auto planets = reg.view<CRenderable, CPlanet, COrbit>();
		const int planetsCount = planets.size();

		char** item_names = new char*[planetsCount + 1];
		for (int i = 0; i < planetsCount; ++i)
			item_names[i] = new char[100];

		int i = 0;
		for (const auto& entity : planets)
		{
			auto& cplanet = planets.get<CPlanet>(entity);
			strcpy(item_names[i++], (std::string("Planet ") + std::to_string(cplanet.id)).c_str());
		}

		static int planet = 0;
		static int planetOld = -1;
		ImGui::Combo("Planets", &planet, item_names, planetsCount, planetsCount);

		static float color1[3] = { 1.0f,1.0f,1.0f };
		static float color2[3] = { 1.0f,1.0f,1.0f };
		static float color3[3] = { 1.0f,1.0f,1.0f };

		glm::vec3 terrainColor1 = glm::vec3(color1[0], color1[1], color1[2]);
		glm::vec3 terrainColor2 = glm::vec3(color2[0], color2[1], color2[2]);
		glm::vec3 oceanColor = glm::vec3(color3[0], color3[1], color3[2]);

		static entt::entity planetEntity = entt::null;
		CPlanet* cplanet = &planets.get<CPlanet>(planets.front());
		static std::uint32_t planetId = std::stoi(std::string(item_names[0]).substr(7));

		planetId = std::stoi(std::string(item_names[planet]).substr(7));
		for (const auto& entity : planets)
		{
			std::uint32_t id = planets.get<CPlanet>(entity).id;
			if (planetId == id)
			{
				cplanet = &planets.get<CPlanet>(entity);
				planetEntity = entity;
			}
		}

		terrainColor1 = cplanet->terrainColor1;
		terrainColor2 = cplanet->terrainColor2;
		oceanColor = cplanet->oceanColor;

		color1[0] = terrainColor1.x;
		color1[1] = terrainColor1.y;
		color1[2] = terrainColor1.z;

		color2[0] = terrainColor2.x;
		color2[1] = terrainColor2.y;
		color2[2] = terrainColor2.z;

		color3[0] = oceanColor.x;
		color3[1] = oceanColor.y;
		color3[2] = oceanColor.z;

		ImGui::ColorEdit3("Terrain 1", color1);
		ImGui::ColorEdit3("Terrain 2", color2);
		ImGui::ColorEdit3("Ocean", color3);

		cplanet->terrainColor1.x = color1[0];
		cplanet->terrainColor1.y = color1[1];
		cplanet->terrainColor1.z = color1[2];

		cplanet->terrainColor2.x = color2[0];
		cplanet->terrainColor2.y = color2[1];
		cplanet->terrainColor2.z = color2[2];

		cplanet->oceanColor.x = color3[0];
		cplanet->oceanColor.y = color3[1];
		cplanet->oceanColor.z = color3[2];

		auto& orbit = planets.get<COrbit>(planetEntity);
		static float angle = orbit.angle;
		static float distance = orbit.distance;
		angle = orbit.angle;
		distance = orbit.distance;

		ImGui::SliderFloat("Angle", &angle, 0.0f, 3.14f * 2.0f);
		ImGui::SliderFloat("Distance", &distance, 0.0f, 100000000000000.0f);

		orbit.angle = angle;
		orbit.distance = distance;

		static bool closest = true;
		ImGui::Checkbox("Select closest planet", &closest);

		if (closest)
		{
			auto& camera = reg.get<CTransform>(reg.view<CController, CTransform>().front());
			glm::vec3 cameraPos = camera.position.getVec3f();
			float distanceMin = glm::distance(cameraPos, reg.get<CTransform>(planets.front()).position.getVec3f());
			int p = 0;
			planet = p;

			for (const auto& entity : planets)
			{
				if (entity == planets.front())
				{
					++p;
					continue;
				}

				float distance = glm::distance(cameraPos, reg.get<CTransform>(entity).position.getVec3f());

				if (distanceMin > distance)
				{
					distanceMin = distance;
					planet = p;
				}
				++p;
			}
		}

		ImGui::End();

		for (int i = 0; i < planetsCount; ++i)
			delete[] item_names[i];
		delete[] item_names;
	};

	return e;
}

entt::entity GUIManager::createGalaxyWindow(entt::registry& reg)
{
	entt::entity e = reg.create();
	auto& galaxy = reg.get<CGalaxy>(reg.view<CGalaxy>().front());

	reg.assign<CRenderable>(e);
	reg.assign<CWindow>(e);
	auto& window = reg.get<CWindow>(e);

	window.draw = [&galaxy]() {
		ImGui::Begin("Galaxy");

		static int stars = galaxy.starCount;
		ImGui::InputInt("Stars", &stars);
		ImGui::SliderInt("Stars Slider", &stars, 0, galaxy.starCountMax);

		if (stars > galaxy.starCountMax) stars = galaxy.starCountMax;
		galaxy.starCount = stars;

		bool pressed = ImGui::Button("Reload Shaders", ImVec2(150, 25));
		if (pressed) Globals::shm->reloadShaders();

		ImGui::End();
	};

	return e;
}