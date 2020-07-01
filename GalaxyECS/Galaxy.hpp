#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <entt/entt.hpp>
#include <kdtree.h>
#include <random>
#include "Components.hpp"
#include "ISystem.hpp"

class Galaxy : public ISystem
{
	struct Point: public std::array<double, 3>
	{
		static const int DIM = 3;

		Point(double x, double y, double z)
		{
			(*this)[0] = x;
			(*this)[1] = y;
			(*this)[2] = z;
		}
	};

	//std::unordered_map<std::shared_ptr<std::vector<Point>>> map;
	std::vector<Point> points;
	kdt::KDTree<Point> kdtree;
	int starCurrent = -1;

	void updateStars(int oldId, int starId, CGalaxy& galaxy);
	void generateSystem(int star, CGalaxy& galaxy);

public:
	Galaxy(entt::registry& registry, const std::uint32_t& priority) : ISystem(registry, priority) {};

	void update(float dt) override;
};