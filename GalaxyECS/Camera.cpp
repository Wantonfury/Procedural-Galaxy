#include "Camera.hpp"

void Camera::update(float dt)
{
	auto view = reg.view<CController, CTransform>();
	if (view.empty()) return;

	auto eCamera = view.front();

	CController& cameraData = view.get<CController>(eCamera);
	CTransform& cameraTransform = view.get<CTransform>(eCamera);

	cameraData.perspective = glm::perspective(cameraData.fov, cameraData.aspect, cameraData.zNear, cameraData.zFar);
	cameraData.PM = cameraData.perspective * glm::mat4_cast(glm::conjugate(cameraTransform.rotation));
}

void Camera::setup(float fov, float aspect, float zNear, float zFar)
{
	auto view = reg.view<CController>();
	if (view.empty()) return;

	CController& camera = view.get<CController>(view.front());
	camera.fov = fov;
	camera.aspect = aspect;
	camera.zNear = zNear;
	camera.zFar = zFar;
}