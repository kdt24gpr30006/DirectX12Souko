#pragma once
#include "../../Math/Matrix/Matrix.h"
#include "../../Math/Vector3/Vector3.h"

class Camera
{
public:
	Camera();
	~Camera();

	static Camera* Main;

	bool Create();

	void Update(
		const Math::Vector3& position,
		const Math::Vector3& target,
		const Math::Vector3& up);

	void SetFieldOfView(float Angle);
	void SetAspect(float Aspect);
	void SetAspect(float Width, float Height);
	void SetZ(float Near, float Far);

	const Math::Matrix& GetView()const;
	const Math::Matrix& GetProjection()const;

	const Math::Vector3& GetPosition()const;
	const Math::Vector3& GetRight();
	const Math::Vector3& GetUp()const;
	const Math::Vector3& GetForward()const;

private:
	Math::Matrix View;
	Math::Matrix Projection;

	Math::Vector3 Position;
	Math::Vector3 Right;
	Math::Vector3 Up;
	Math::Vector3 Forward;

	float Fov;
	float Aspect;
	float ZNear;
	float ZFar;
};

