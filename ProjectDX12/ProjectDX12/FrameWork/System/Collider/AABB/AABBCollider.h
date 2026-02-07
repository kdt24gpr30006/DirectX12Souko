#pragma once
#include "Math/Vector3/Vector3.h"

/// <summary>
/// Axis-Aligned Bounding Boxes(座標軸に沿ったバウンディングボックス)
/// 回転しないボックス用コリジョン
/// </summary>
class AABBCollider
{
public:

	AABBCollider();
	~AABBCollider();

	struct Range
	{
		float Min;
		float Max;
	};

	void DebugRender();

	/// <summary>
	/// 接触の判定
	/// </summary>
	/// <param name="other"></param>
	/// <returns>true:接触</returns>
	bool CheckHit(const AABBCollider* other,Math::Vector3& outVector)const;

	/// <summary>
	/// 中心座標の指定
	/// </summary>
	/// <param name="center">中心座標</param>
	void SetCenter(const Math::Vector3& center);

	/// <summary>
	/// 各軸の大きさの指定
	/// </summary>
	/// <param name="volume">各軸の大きさ</param>
	void SetVolume(const Math::Vector3& volume);

	/// <summary>
	/// X軸の範囲の取得
	/// </summary>
	/// <returns>X軸の範囲</returns>
	Range GetAxisXRange() const;

	/// <summary>
	/// Y軸の範囲の取得
	/// </summary>
	/// <returns>Y軸の範囲</returns>
	Range GetAxisYRange() const;

	/// <summary>
	/// Z軸の範囲の取得
	/// </summary>
	/// <returns>Z軸の範囲</returns>
	Range GetAxisZRange()const;

	/// <summary>
	/// レイキャスト判定（Slab法）
	/// </summary>
	/// <param name="origin">レイの始点</param>
	/// <param name="direction">レイの方向（正規化済み）</param>
	/// <param name="maxDistance">レイの最大距離</param>
	/// <param name="outHitDistance">ヒットまでの距離（出力）</param>
	/// <returns>true:ヒット</returns>
	bool CastRay(
		const Math::Vector3& origin,
		const Math::Vector3& direction,
		float maxDistance,
		float& outHitDistance) const;

private:

	/// <summary>
	/// 中心座標
	/// </summary>
	Math::Vector3 Center;

	/// <summary>
	/// 各軸の大きさ
	/// </summary>
	Math::Vector3 Volume;
};

