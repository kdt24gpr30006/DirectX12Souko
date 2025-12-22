#include "Camera.h"
#include "../../Math/Math.h"

Camera* Camera::Main = nullptr;

Camera::Camera()
	: View()
	, Projection()
	, Position()
	, Right()
	, Up()
	, Forward()
	, Fov()
	, Aspect()
	, ZNear()
	, ZFar()
{

}
Camera::~Camera()
{

}

bool Camera::Create()
{
	SetFieldOfView(90.0f);
	SetAspect(16.0f, 9.0f);
	SetZ(0.1f, 1000.0f);
	Main = this;
	return true;
}

void Camera::Update(
	const Math::Vector3& position,
	const Math::Vector3& target,
	const Math::Vector3& up)
{
	Position = position;
	//	カメラ(View)行列設定
	View = Math::Matrix::LookAt(position, target, up);

	//	カメラの右ベクトル更新
	Right.x = View._11;
	Right.y = View._21;
	Right.z = View._31;

	//	カメラの上ベクトル更新
	Up.x = View._12;
	Up.y = View._22;
	Up.z = View._32;

	//	カメラの前ベクトル更新
	Forward.x = View._13;
	Forward.y = View._23;
	Forward.z = View._33;

	//	プロジェクション行列設定
	//Projection = Math::Matrix::PerspectiveFovRH(Fov, Aspect, ZNear, ZFar);
	Projection = Math::Matrix::PerspectiveFovLH(Fov, Aspect, ZNear, ZFar);
}

void Camera::SetFieldOfView(float Angle)
{
	Fov = Angle * Math::RAD;
}

void Camera::SetAspect(float Aspect)
{
	this->Aspect = Aspect;
}
void Camera::SetAspect(float Width, float Height)
{
	Aspect = Width / Height;
}

void Camera::SetZ(float Near, float Far)
{
	ZNear = Near;
	ZFar = Far;
}

const Math::Matrix& Camera::GetView()const
{
	return View;
}
const Math::Matrix& Camera::GetProjection()const
{
	return Projection;
}

const Math::Vector3& Camera::GetPosition()const
{
	return Position;
}
const Math::Vector3& Camera::GetRight()
{
	return Right;
}
const Math::Vector3& Camera::GetUp()const
{
	return Up;
}

const Math::Vector3& Camera::GetForward()const
{
	return Forward;
}
