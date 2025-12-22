#pragma once
#include "Math/Vector3/Vector3.h"

/// <summary>
/// 球体コリジョン用クラス
/// </summary>
class SphereCollider
{
public:
	SphereCollider();
	~SphereCollider();

	/// <summary>
	/// 接触の判定
	/// </summary>
	/// <param name="other"></param>
	/// <returns></returns>
	bool CheckHit(const SphereCollider* other)const;
	
	/// <summary>
	/// 中心座標の取得
	/// </summary>
	/// <returns>中心座標</returns>
	const Math::Vector3& GetCenter()const;

	/// <summary>
	/// 中心座標の指定
	/// </summary>
	/// <param name="center">中心座標</param>
	void SetCenter(const Math::Vector3& center);

	/// <summary>
	/// 半径の取得
	/// </summary>
	/// <returns>半径</returns>
	float GetRadius()const;

	/// <summary>
	/// 半径の指定
	/// </summary>
	/// <param name="radius">半径</param>
	void SetRadius(float radius);
private:

	/// <summary>
	/// 中心座標
	/// </summary>
	Math::Vector3 Center;

	/// <summary>
	/// 半径
	/// </summary>
	float Radius;
};