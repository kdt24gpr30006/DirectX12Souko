#pragma once
#include "Math/Vector3/Vector3.h"

class Camera;

class CameraWork
{
public:
	CameraWork();
	~CameraWork();

	bool Initialize();
	void Release();
	void Update();
	void Render();

	void SetTarget(const Math::Vector3& target)
	{
		Target = target;
	}

private:
	Camera* camera;

	Math::Vector3 Target;

	float Angle;
};

