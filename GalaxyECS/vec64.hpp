#pragma once

#include <cstdlib>
#include <memory>
#include <glm/glm.hpp>
#include <iostream>

class vec64
{
public:
	std::int64_t x, y, z;

	const static float delimiter;
	const static std::int64_t delimiter64;

	vec64();
	vec64(std::int64_t v);
	vec64(std::int64_t x, std::int64_t y, std::int64_t z);
	vec64(float x, float y, float z);
	vec64(const vec64& v);
	vec64(glm::vec3);

	static std::int64_t floatToInt64(float v);

	float getX() const;
	float getY() const;
	float getZ() const;

	void setX(float x);
	void setX(std::int64_t x);
	void setY(float y);
	void setY(std::int64_t y);
	void setZ(float z);
	void setZ(std::int64_t z);

	glm::vec3 getVec3f() const;

	vec64& operator=(const vec64& v);
	vec64 operator*(float v);
	vec64 operator-(const vec64& v);
	vec64 operator+(const glm::vec3& v);
	vec64 operator+(const vec64& v);
	vec64 operator+(const float v);

	friend std::ostream& operator<<(std::ostream& os, const vec64& v);
};