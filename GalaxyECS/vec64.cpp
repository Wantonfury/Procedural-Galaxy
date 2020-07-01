#include "vec64.hpp"

const float vec64::delimiter = 1000000.0f;//10000000000.0f; // 10000.0f;
const std::int64_t vec64::delimiter64 = 1000000;//std::numeric_cast<int64_t, float>(vec64::delimiter);

vec64::vec64()
{
	x = 0;
	y = 0;
	z = 0;
}

vec64::vec64(std::int64_t v)
{
	this->x = v;
	this->y = v;
	this->z = v;
}

vec64::vec64(std::int64_t x, std::int64_t y, std::int64_t z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

vec64::vec64(float x, float y, float z)
{
	this->x = floatToInt64(x);
	this->y = floatToInt64(y);
	this->z = floatToInt64(z);
}

vec64::vec64(const vec64& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

vec64::vec64(glm::vec3 v)
{
	x = floatToInt64(v.x);
	y = floatToInt64(v.y);
	z = floatToInt64(v.z);
}


std::int64_t vec64::floatToInt64(float v)
{
	v = v * vec64::delimiter;
	std::int64_t w = static_cast<std::int64_t>(v);

	return w;
}


float vec64::getX() const
{
	return static_cast<float>(x) / delimiter;
}

float vec64::getY() const
{
	return static_cast<float>(y) / delimiter;
}

float vec64::getZ() const
{
	return static_cast<float>(z) / delimiter;
}


void vec64::setX(float x)
{
	this->x = floatToInt64(x);
}

void vec64::setX(std::int64_t x)
{
	this->x = x;
}

void vec64::setY(float y)
{
	this->y = floatToInt64(y);
}

void vec64::setY(std::int64_t y)
{
	this->y = y;
}

void vec64::setZ(float z)
{
	this->z = floatToInt64(z);
}

void vec64::setZ(std::int64_t z)
{
	this->z = z;
}


glm::vec3 vec64::getVec3f() const
{
	glm::vec3 v(getX(), getY(), getZ());
	return v;
}


vec64& vec64::operator=(const vec64& v)
{
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
	return *this;
}

vec64 vec64::operator*(float v)
{
	vec64 vec(*this);
	std::int64_t num = floatToInt64(v);

	vec.x *= num;
	vec.y *= num;
	vec.z *= num;

	return vec;
}

vec64 vec64::operator-(const vec64& v)
{
	vec64 vec(*this);

	vec.x -= v.x;
	vec.y -= v.y;
	vec.z -= v.z;

	return vec;
}

vec64 vec64::operator+(const glm::vec3& v)
{
	vec64 vec(*this);

	vec.x += floatToInt64(v.x);
	vec.y += floatToInt64(v.y);
	vec.z += floatToInt64(v.z);

	return vec;
}

vec64 vec64::operator+(const vec64& v)
{
	vec64 vec(*this);

	vec.x += v.x;
	vec.y += v.y;
	vec.z += v.z;

	return vec;
}

vec64 vec64::operator+(const float v)
{
	vec64 vec(*this);
	std::int64_t num = floatToInt64(v);

	vec.x += num;
	vec.y += num;
	vec.z += num;

	return vec;
}

std::ostream& operator<<(std::ostream& os, const vec64& v)
{
	os << v.x << ", " << v.y << ", " << v.z;
	return os;
}