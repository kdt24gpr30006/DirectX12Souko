#pragma once
#include "Math/Vector3/Vector3.h"
#include "Math/Quaternion/Quaternion.h"

/// <summary>
/// Oriented Bounding Box(指向性のバウンディングボックス)
/// 回転を考慮したボックス用コリジョン
/// </summary>
class OBBCollider
{
public:
	OBBCollider();
	~OBBCollider();

	/// <summary>
	/// 接触の判定
	/// </summary>
	/// <param name="other"></param>
	/// <returns>true:接触</returns>
	bool CheckHit(const OBBCollider* other)const;

	/// <summary>
	/// レイによる接触判定
	/// </summary>
	/// <param name="normalizedDirection">正規化されたレイの方向ベクトル</param>
	/// <param name="position">レイの座標</param>
	/// <param name="distance">レイの範囲</param>
	/// <param name="hitPosition">当たった時の座標</param>
	/// <returns>true:接触</returns>
	bool CastRay(
		const Math::Vector3& normalizedDirection,
		const Math::Vector3& position,
		float distance,
		Math::Vector3& hitPosition)const;

	/// <summary>
	/// 中心の座標の取得
	/// </summary>
	/// <returns>中心の座標</returns>
	const Math::Vector3& GetCenter()const;

	/// <summary>
	/// 中心座標の指定
	/// </summary>
	/// <param name="center">中心座標</param>
	void SetCenter(const Math::Vector3& center);

	/// <summary>
	/// 各軸の大きさの取得
	/// </summary>
	/// <returns>各軸の大きさ</returns>
	const Math::Vector3& GetVolume()const;

	/// <summary>
	/// 各軸の大きさの指定
	/// </summary>
	/// <param name="volume">各軸の大きさ</param>
	void SetVolume(const Math::Vector3& volume);

	/// <summary>
	/// オブジェクトの回転の取得
	/// </summary>
	/// <returns>オブジェクトの回転</returns>
	const Math::Quaternion& GetRotation()const;

	/// <summary>
	/// オブジェクトの回転の指定
	/// </summary>
	/// <param name="rotation">オブジェクトの回転</param>
	void SetRotation(const Math::Quaternion& rotation);

private:

	Math::Vector3 Center;

	Math::Vector3 Volume;

	Math::Quaternion Rotation;
};

