#include "Orbit.h"

void Orbit::update(float dt)
{
	reg.sort<COrbit>([](const auto& a, const auto& b)
	{
		return a.distance > b.distance;
	});

	auto view = reg.view<CTransform, COrbit>();
	for (const auto& entity : view)
	{
		auto& orbit = view.get<COrbit>(entity);
		auto& transform = view.get<CTransform>(entity);

		if (!reg.valid(orbit.target)) continue;

		auto& transformTarget = reg.get<CTransform>(orbit.target);
		transform.position = transformTarget.position;

		if (orbit.distance > 0.0f)
		{
			transform.position.x -= cos(orbit.angle) * orbit.distance;
			transform.position.z -= sin(orbit.angle) * orbit.distance;
			orbit.angle += orbit.speed * dt;
		}
	}
}