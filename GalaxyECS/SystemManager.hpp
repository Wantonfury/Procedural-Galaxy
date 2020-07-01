#pragma once

#include <entt/entt.hpp>
#include <cstdlib>
#include <memory>
#include <unordered_map>
#include <array>
#include <string>
#include "Assert.hpp"
#include "ISystem.hpp"

class SystemManager
{
	static const std::uint32_t MAX_SYSTEMS = 32;
	std::uint32_t systemCount = 0;

	std::array<std::shared_ptr<ISystem>, MAX_SYSTEMS> systems;
	std::unordered_map<const char*, std::shared_ptr<ISystem>> indexToSystem;

	static bool sortByPriority(const std::shared_ptr<ISystem>& a, const std::shared_ptr<ISystem>& b);

public:
	void update(float dt);

	template <typename T>
	std::shared_ptr<T> registerSystem(entt::registry& reg, const std::uint32_t& priority);

	template <typename T>
	void removeSystem();

	template <typename T>
	std::shared_ptr<T> getSystem();

	template <typename T>
	bool hasSystem();
};

bool SystemManager::sortByPriority(const std::shared_ptr<ISystem>& a, const std::shared_ptr<ISystem>& b)
{
	return a->getPriority() > b->getPriority();
}

void SystemManager::update(float dt)
{
	// Sort systems so that they're always called in the correct priority order (a > b > c)
	std::sort(systems.begin(), systems.begin() + systemCount, sortByPriority);

	for (std::uint32_t i = 0; i < systemCount; ++i)
		systems[i]->update(dt);
}

template <typename T>
std::shared_ptr<T> SystemManager::registerSystem(entt::registry& reg, const std::uint32_t& priority)
{
	assertFunc(systemCount < MAX_SYSTEMS, [&]() {
		std::cerr << "Error: Too many systems registered. (" + std::to_string(systemCount) + " out of " + std::to_string(MAX_SYSTEMS) + ")" << std::endl;
	});

	const char *index = typeid(T).name();
	if (indexToSystem.find(index) != indexToSystem.end()) return std::dynamic_pointer_cast<T>(indexToSystem[index]);

	systems[systemCount] = std::make_shared<T>(reg, priority);
	indexToSystem.insert({ index, systems[systemCount] });

	return std::dynamic_pointer_cast<T>(systems[systemCount++]);
}

template <typename T>
void SystemManager::removeSystem()
{
	const char* index = typeid(T).name();
	if (indexToSystem.find(index) == indexToSystem.end()) return;

	for (std::uint32_t i = 0, k = 0; i < systemCount && k < systemCount; ++i, ++k)
	{
		if (i != k) systems[k].swap(systems[i]);

		if (i == k && std::strcmp(index, typeid(*systems[i]).name()) == 0)
			systems[k--].reset();
	}

	--systemCount;
	indexToSystem.erase(index);
}

template <typename T>
std::shared_ptr<T> SystemManager::getSystem()
{
	const char* index = typeid(T).name();

	if (indexToSystem.find(index) == indexToSystem.end()) return nullptr;
	return std::dynamic_pointer_cast<T>(indexToSystem[index]);
}

template <typename T>
bool SystemManager::hasSystem()
{
	if (indexToSystem.find(typeid(T).name()) == indexToSystem.end()) return false;
	return true;
}