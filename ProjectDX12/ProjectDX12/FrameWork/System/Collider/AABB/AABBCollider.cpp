#include "AABBCollider.h"
#include "Graphics/Graphics.h"
#include "Graphics/Renderer/Renderer.h"

AABBCollider::AABBCollider()
	: Center()
	, Volume()
{
}
AABBCollider::~AABBCollider()
{
}

/// <summary>
/// 接触の判定
/// </summary>
/// <param name="other"></param>
/// <returns>true:接触</returns>
bool AABBCollider::CheckHit(const AABBCollider* other, Math::Vector3& outVector)const
{
	outVector.x = 0.0f;
	outVector.y = 0.0f;
	outVector.z = 0.0f;

	const Range MyX = GetAxisXRange();
	const Range MyY = GetAxisYRange();
	const Range MyZ = GetAxisZRange();
	const Range OtherX = other->GetAxisXRange();
	const Range OtherY = other->GetAxisYRange();
	const Range OtherZ = other->GetAxisZRange();

	const bool hit =
		MyX.Min < OtherX.Max &&
		MyX.Max > OtherX.Min &&
		MyY.Min < OtherY.Max &&
		MyY.Max > OtherY.Min &&
		MyZ.Min < OtherZ.Max &&
		MyZ.Max > OtherZ.Min;
	if (hit == true)
	{
		const Math::Vector3 toOther = other->Center - Center;
		const float x = Volume.x * 0.5f + other->Volume.x * 0.5f;
		const float y = Volume.y * 0.5f + other->Volume.y * 0.5f;
		const float z = Volume.z * 0.5f + other->Volume.z * 0.5f;
		//	入った分の計算
		outVector.x = x - fabsf(toOther.x);
		outVector.y = y - fabsf(toOther.y);
		outVector.z = z - fabsf(toOther.z);
	}
	return hit;
}

void AABBCollider::DebugRender()
{
	Graphics::LineVertex vertices[8];
	for (auto& v : vertices)
	{
		v.color = Color::Yellow;
	}

	const Math::Vector3 Right = Math::Vector3(Volume.x * 0.5f, 0.0f, 0.0f);
	const Math::Vector3 Up = Math::Vector3(0.0f, Volume.y * 0.5f, 0.0f);
	const Math::Vector3 Forward = Math::Vector3(0.0f, 0.0f, Volume.z * 0.5f);
	//	上面
	//	左手前
	vertices[0].Position = -Right + Up + Forward + Center;
	//	左奥
	vertices[1].Position = -Right + Up + -Forward + Center;
	//	右手前
	vertices[2].Position = Right + Up + Forward + Center;
	//	右奥
	vertices[3].Position = Right + Up + -Forward + Center;
	//	底面
	//	左手前
	vertices[4].Position = -Right + -Up + Forward + Center;
	//	左奥
	vertices[5].Position = -Right + -Up + -Forward + Center;
	//	右手前
	vertices[6].Position = Right + -Up + Forward + Center;
	//	右奥
	vertices[7].Position = Right + -Up + -Forward + Center;
		
	const uint16_t indices[24] =
	{
		//上面
		0,1,
		0,2,
		1,3,
		2,3,
		//縦線
		0,4,
		1,5,
		3,7,
		2,6,
		//底面
		4,5,
		4,6,
		5,7,
		6,7
	};

	Graphics::Renderer* renderer = Graphics::Renderer::GetInstance();
	renderer->DrawLines(vertices, 8, indices, 24);
}

/// <summary>
/// 中心座標の指定
/// </summary>
/// <param name="center">中心座標</param>
void AABBCollider::SetCenter(const Math::Vector3& center)
{
	Center = center;
}

/// <summary>
/// 各軸の大きさの指定
/// </summary>
/// <param name="volume">各軸の大きさ</param>
void AABBCollider::SetVolume(const Math::Vector3& volume)
{
	Volume = volume;
}

/// <summary>
/// X軸の範囲の取得
/// </summary>
/// <returns>X軸の範囲</returns>
AABBCollider::Range AABBCollider::GetAxisXRange() const
{
	const float HalfVolume = Volume.x / 2.0f;
	Range range = {};
	range.Min = Center.x - HalfVolume;
	range.Max = Center.x + HalfVolume;
	return range;
}

/// <summary>
/// Y軸の範囲の取得
/// </summary>
/// <returns>Y軸の範囲</returns>
AABBCollider::Range AABBCollider::GetAxisYRange() const
{
	const float HalfVolume = Volume.y / 2.0f;
	Range range = {};
	range.Min = Center.y - HalfVolume;
	range.Max = Center.y + HalfVolume;
	return range;
}

/// <summary>
/// Z軸の範囲の取得
/// </summary>
/// <returns>Z軸の範囲</returns>
AABBCollider::Range AABBCollider::GetAxisZRange() const
{
	const float HalfVolume = Volume.z / 2.0f;
	Range range = {};
	range.Min = Center.z - HalfVolume;
	range.Max = Center.z + HalfVolume;
	return range;
}