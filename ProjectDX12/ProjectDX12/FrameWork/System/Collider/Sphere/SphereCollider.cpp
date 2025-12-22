#include "SphereCollider.h"

SphereCollider::SphereCollider()
	: Center()
	, Radius()
{

}
SphereCollider::~SphereCollider()
{
}

/// <summary>
/// 接触の判定
/// </summary>
/// <param name="other"></param>
/// <returns></returns>
bool SphereCollider::CheckHit(const SphereCollider* other)const
{
#if true
	const float distance = Math::Vector3::Distance(GetCenter(), other->GetCenter());
	const float rad = GetRadius() + other->GetRadius();
	
	bool hit = false;
	if (distance <= rad)
	{
		hit = true;
	}
	return hit;
#else
	const Math::Vector3 v = GetCenter() - other->GetCenter();
	const float dis2 = v.SqrLength();
	const float rad = GetRadius() + other->GetRadius();

	bool hit = false;
	if (dis2 <= rad * rad)
	{
		hit = true;
	}
	return hit;
#endif
}

/// <summary>
/// 中心座標の取得
/// </summary>
/// <returns>中心座標</returns>
const Math::Vector3& SphereCollider::GetCenter()const
{
	return Center;
}

/// <summary>
/// 中心座標の指定
/// </summary>
/// <param name="center">中心座標</param>
void SphereCollider::SetCenter(const Math::Vector3& center)
{
	Center = center;
}

/// <summary>
/// 半径の取得
/// </summary>
/// <returns>半径</returns>
float SphereCollider::GetRadius()const
{
	return Radius;
}

/// <summary>
/// 半径の指定
/// </summary>
/// <param name="radius">半径</param>
void SphereCollider::SetRadius(float radius)
{
	Radius = radius;
}