#include "ISystem.hpp"

void ISystem::setPriority(const std::uint32_t& priority)
{
	this->priority = priority;
}

std::uint32_t ISystem::getPriority()
{
	return priority;
}