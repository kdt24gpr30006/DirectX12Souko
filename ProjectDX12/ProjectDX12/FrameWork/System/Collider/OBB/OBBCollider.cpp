#include "OBBCollider.h"
#include "Math/Matrix/Matrix.h"

OBBCollider::OBBCollider()
	: Center(0,0,0)
	, Volume(1.0f, 1.0f, 1.0f)
	, Rotation(Math::Quaternion::Identity)
{
}
OBBCollider::~OBBCollider()
{
}

/// <summary>
/// 接触の判定
/// </summary>
/// <param name="other"></param>
/// <returns>true:接触</returns>
bool OBBCollider::CheckHit(const OBBCollider* other)const
{
	//	自分の情報
	const Math::Matrix MyTransform = GetRotation().ToMatrix();
	const Math::Vector3 MyHalfVolume = GetVolume() * 0.5f;
	//	正規化 右ベクトル(Right Vector)
	const Math::Vector3 MyNormalizedRV = Math::Vector3::Normalize(MyTransform.GetRightVector());
	const Math::Vector3 MyRV = MyNormalizedRV * MyHalfVolume.x;
	//	正規化 上ベクトル(Up Vector)
	const Math::Vector3 MyNormalizedUV = Math::Vector3::Normalize(MyTransform.GetUpVector());
	const Math::Vector3 MyUV = MyNormalizedUV * MyHalfVolume.y;
	//	正規化 前ベクトル(Forward Vector)
	const Math::Vector3 MyNormalizedFV = Math::Vector3::Normalize(MyTransform.GetForwardVector());
	const Math::Vector3 MyFV = MyNormalizedFV * MyHalfVolume.z;

	//	相手の情報
	const Math::Matrix OtherTransform = other->GetRotation().ToMatrix();
	const Math::Vector3 OtherHalfVolume = GetVolume() * 0.5f;
	//	正規化 右ベクトル(Right Vector)
	const Math::Vector3 OtherNormalizedRV = Math::Vector3::Normalize(OtherTransform.GetRightVector());
	const Math::Vector3 OtherRV = OtherNormalizedRV * OtherHalfVolume.x;
	//	正規化 上ベクトル(Up Vector)
	const Math::Vector3 OtherNormalizedUV = Math::Vector3::Normalize(OtherTransform.GetUpVector());
	const Math::Vector3 OtherUV = OtherNormalizedUV * OtherHalfVolume.y;
	//	正規化 前ベクトル(Forward Vector)
	const Math::Vector3 OtherNormalizedFV = Math::Vector3::Normalize(OtherTransform.GetForwardVector());
	const Math::Vector3 OtherFV = OtherNormalizedFV * OtherHalfVolume.z;


	//	自分の中心から相手の中心へのベクトル
	const Math::Vector3 MyCenter = GetCenter();
	const Math::Vector3 OtherCenter = other->GetCenter();
	const Math::Vector3 ToOtherVector = OtherCenter - MyCenter;
	const Math::Vector3 ToOtherNormalizedVector = Math::Vector3::Normalize(ToOtherVector);

	auto CalcDotLength = [](const Math::Vector3& NormalizedVector, const Math::Vector3& RV, const Math::Vector3& UV, const Math::Vector3& FV)
		{
			return fabsf(Math::Vector3::Dot(NormalizedVector, RV)) +
				fabsf(Math::Vector3::Dot(NormalizedVector, UV)) +
				fabsf(Math::Vector3::Dot(NormalizedVector, FV));
		};

	//	自分の右方向ベクトル(X軸)の大きさ(長さ)
	float MyXSize = MyRV.Length();
	float OtherXSize = CalcDotLength(MyNormalizedRV, OtherRV, OtherUV, OtherFV);
	//	自分のX軸方向のサイズと相手のX軸方向のサイズとの比較
	float Distance = fabsf(Math::Vector3::Dot(ToOtherVector, MyNormalizedRV));
	if (Distance > MyXSize + OtherXSize)
	{
		//	重なっていない
		return false;
	}

	//	自分の上方向(Y軸)の大きさ(長さ)
	float MyYSize = MyUV.Length();
	float OtherYSize = CalcDotLength(MyNormalizedUV, OtherRV, OtherUV, OtherFV);
	//	自分のY軸方向のサイズと相手のY軸方向のサイズとの比較
	Distance = fabsf(Math::Vector3::Dot(ToOtherVector, MyNormalizedUV));
	if (Distance > MyYSize + OtherYSize)
	{
		//	重なっていない
		return false;
	}

	//	自分の前方向(Z軸)の大きさ(長さ)
	float MyZSize = MyFV.Length();
	float OtherZSize = CalcDotLength(MyNormalizedFV, OtherRV, OtherUV, OtherFV);
	//	自分のZ軸方向のサイズと相手のZ軸方向のサイズとの比較
	Distance = fabsf(Math::Vector3::Dot(ToOtherVector, MyNormalizedFV));
	if (Distance > MyZSize + OtherZSize)
	{
		//	重なっていない
		return false;
	}

	MyXSize = CalcDotLength(OtherNormalizedRV, MyRV, MyUV, MyFV);
	//	相手の右方向(X軸)の大きさ(長さ)
	OtherXSize = OtherRV.Length();
	//	自分のX軸方向のサイズと相手のX軸方向のサイズとの比較
	Distance = fabsf(Math::Vector3::Dot(ToOtherVector, OtherNormalizedRV));
	if (Distance > MyXSize + OtherXSize)
	{
		//	重なっていない
		return false;
	}

	MyYSize = CalcDotLength(OtherNormalizedUV, MyRV, MyUV, MyFV);
	//	相手の上方向(Y軸)の大きさ(長さ)
	OtherYSize = OtherUV.Length();
	//	自分のY軸方向のサイズと相手のY軸方向のサイズとの比較
	Distance = fabsf(Math::Vector3::Dot(ToOtherVector, OtherNormalizedUV));
	if (Distance > MyYSize + OtherYSize)
	{
		//	重なっていない
		return false;
	}

	MyZSize = CalcDotLength(OtherNormalizedFV, MyRV, MyUV, MyFV);
	//	相手の前方向(Z軸)の大きさ(長さ)
	OtherZSize = OtherFV.Length();
	//	自分のZ軸方向のサイズと相手のZ軸方向のサイズとの比較
	Distance = fabsf(Math::Vector3::Dot(ToOtherVector, OtherNormalizedFV));
	if (Distance > MyZSize + OtherZSize)
	{
		//	重なっていない
		return false;
	}


	//	双方の方向ベクトルの外積を計算し、投影が重なっているか判定を行う(9回)
	struct Pair
	{
		Math::Vector3 MyDirectionVector;
		Math::Vector3 OtherDirectionVector;
	};

	Pair pair[9] =
	{
		{ MyNormalizedRV, OtherNormalizedRV },
		{ MyNormalizedRV, OtherNormalizedUV },
		{ MyNormalizedRV, OtherNormalizedFV },

		{ MyNormalizedUV, OtherNormalizedRV },
		{ MyNormalizedUV, OtherNormalizedUV },
		{ MyNormalizedUV, OtherNormalizedFV },

		{ MyNormalizedFV, OtherNormalizedRV },
		{ MyNormalizedFV, OtherNormalizedUV },
		{ MyNormalizedFV, OtherNormalizedFV }
	};

	for (const auto& itr : pair)
	{
		const Math::Vector3 Cross = Math::Vector3::Cross(itr.MyDirectionVector, itr.OtherDirectionVector);
		const float MySize = CalcDotLength(Cross, MyRV, MyUV, MyFV);
		const float OtherSize = CalcDotLength(Cross, OtherRV, OtherUV, OtherFV);
		Distance = fabsf(Math::Vector3::Dot(ToOtherVector, Cross));
		if (Distance > MyZSize + OtherZSize)
		{
			//	重なっていない
			return false;
		}
	}
	
	//	全ての射影が重なっていた(接触)
	return true;
}

/// <summary>
/// レイによる接触判定
/// </summary>
/// <param name="normalizedDirection">正規化されたレイの方向ベクトル</param>
/// <param name="position">レイの座標</param>
/// <param name="distance">レイの範囲</param>
/// <param name="hitPosition">当たった時の座標</param>
/// <returns>true:接触</returns>
bool OBBCollider::CastRay(
	const Math::Vector3& normalizedDirection,
	const Math::Vector3& position,
	float distance,
	Math::Vector3& hitPosition)const
{
	const Math::Matrix Transform = GetRotation().ToMatrix();
	const Math::Vector3 HalfVolume = GetVolume() * 0.5f;
	//	右ベクトル(Right Vector)
	Math::Vector3 Right = Transform.GetRightVector();
	Right.Normalize();
	Right *= HalfVolume.x;
	//	上ベクトル(Up Vector)
	Math::Vector3 Up = Transform.GetUpVector();
	Up.Normalize();
	Up *= HalfVolume.y;
	//	前ベクトル(Forward Vector)
	Math::Vector3 Forward = Transform.GetForwardVector();
	Forward.Normalize();
	Forward *= HalfVolume.z;

	//	ボックスにレイを放つので8頂点
	static constexpr int VertexCount = 8;
	const Math::Vector3 Vertices[VertexCount] =
	{
		//	上面
		//	左手前
		{ -Right + Up + Forward + Center},
		//	左奥
		{ -Right + Up + -Forward + Center},
		//	右手前
		{ Right + Up + Forward + Center},
		//	右奥
		{ Right + Up + -Forward + Center},

		//	底面
		//	左手前
		{ -Right + -Up + Forward + Center},
		//	左奥
		{ -Right + -Up + -Forward + Center},
		//	右手前
		{ Right + -Up + Forward + Center},
		//	右奥
		{ Right + -Up + -Forward + Center}
	};
	
	//	1面に2ポリゴン の6面で 12ポリゴン
	static constexpr int PolygonCount = 2 * 6;
	static constexpr int IndexCount = PolygonCount * 3;
	auto GetVertexIndex = [](int VertexIndex)
		{
			constexpr int Indices[IndexCount] =
			{
				0, 2, 1, //上面1
				1, 2, 3, //上面2
				0, 4, 2, //手前面1
				2, 4, 6, //手前面2
				0, 1, 4, //左側面1
				1, 5, 4, //左側面2
				2, 6, 3, //右側面1
				3, 6, 7, //右側面2
				1, 3, 5, //奥面1
				3, 7, 5, //奥面2
				4, 5, 6, //底面1
				5, 7, 6, //底面2
			};
			return Indices[VertexIndex];
		};

	bool bIsHit = false;
	float NearDistance = distance;
	for (int index = 0; index < IndexCount; index += 3)
	{
		const int i0 = GetVertexIndex(index);
		const int i1 = GetVertexIndex(index + 1);
		const int i2 = GetVertexIndex(index + 2);

		const Math::Vector3& v0 = Vertices[i0];
		const Math::Vector3& v1 = Vertices[i1];
		const Math::Vector3& v2 = Vertices[i2];

		//	各頂点の線分
		const Math::Vector3 line0 = v1 - v0;
		const Math::Vector3 line1 = v2 - v1;
		const Math::Vector3 line2 = v0 - v2;

		//	面の法線
		const Math::Vector3 normal = Math::Vector3::Cross(line0, line1);
		const float dot = Math::Vector3::Dot(normal, normalizedDirection);
		if (dot >= 0)
		{
			//	レイの方向と面の法線が向き合っていない(同じ方向なら裏面)
			continue;
		}

		//	レイの座標から頂点へのベクトル
		const Math::Vector3 vec = v0 - position;
		const float Coef = Math::Vector3::Dot(normal, vec);

		//	交点までの距離
		const float t = Coef / dot;

		//	方向ベクトルと法線の内積で マイナスの値
		//	レイ座標から頂点へのベクトルと法線の内積で マイナスの値
		if (t < 0)
		{
			//	プラスの値は対象外
			continue;
		}

		if (t >= NearDistance)
		{
			//	距離範囲外 もしくは 最も近い点 ではない
			continue;
		}

		const Math::Vector3 point = position + normalizedDirection * t;

		//	レイのポイントが3角形の中に存在するかチェック
		const Math::Vector3 p0 = point - v0;
		const Math::Vector3 n0 = Math::Vector3::Cross(line2, p0);
		const float dot0 = Math::Vector3::Dot(n0, normal);
		if (dot0 < 0)
		{
			//	面の法線と反対方向はNG
			continue;
		}

		const Math::Vector3 p1 = point - v1;
		const Math::Vector3 n1 = Math::Vector3::Cross(line0, p1);
		const float dot1 = Math::Vector3::Dot(n1, normal);
		if (dot1 < 0)
		{
			//	面の法線と反対方向はNG
			continue;
		}

		const Math::Vector3 p2 = point - v2;
		const Math::Vector3 n2 = Math::Vector3::Cross(line1, p2);
		const float dot2 = Math::Vector3::Dot(n2, normal);
		if (dot2 < 0)
		{
			//	面の法線と反対方向はNG
			continue;
		}

		//	レイが面にヒットした
		NearDistance = t;
		hitPosition = point;
		bIsHit = true;
	}

	return bIsHit;
}


/// <summary>
/// 中心の座標の取得
/// </summary>
/// <returns>中心の座標</returns>
const Math::Vector3& OBBCollider::GetCenter()const
{
	return Center;
}

/// <summary>
/// 中心座標の指定
/// </summary>
/// <param name="center">中心座標</param>
void OBBCollider::SetCenter(const Math::Vector3& center)
{
	Center = center;
}

/// <summary>
/// 各軸の大きさの取得
/// </summary>
/// <returns>各軸の大きさ</returns>
const Math::Vector3& OBBCollider::GetVolume()const
{
	return Volume;
}

/// <summary>
/// 各軸の大きさの指定
/// </summary>
/// <param name="volume">各軸の大きさ</param>
void OBBCollider::SetVolume(const Math::Vector3& volume)
{
	Volume = volume;
}

/// <summary>
/// オブジェクトの回転の取得
/// </summary>
/// <returns>オブジェクトの回転</returns>
const Math::Quaternion& OBBCollider::GetRotation()const
{
	return Rotation;
}

/// <summary>
/// オブジェクトの回転の指定
/// </summary>
void OBBCollider::SetRotation(const Math::Quaternion& rotation)
{
	Rotation = rotation;
}