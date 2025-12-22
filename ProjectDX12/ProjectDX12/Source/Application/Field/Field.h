#pragma once
#include <array>
#include "Math/Vector3/Vector3.h"

class FbxMesh;
class AABBCollider;

class Field
{
public:
	Field();
	~Field();

	bool Initialize();
	void Release();
	void Update();
	void Render();

	bool CheckFieldCollider(const AABBCollider* other, Math::Vector3& outVector)const;
	bool CheckFloorCollider(const AABBCollider* other, Math::Vector3& outVector)const;
private:
	static constexpr int NumHalfSize = 15;
	std::array<FbxMesh*, NumHalfSize * NumHalfSize> Boxes;
	std::array<AABBCollider*, NumHalfSize * NumHalfSize> BoxColliders;

	FbxMesh* Floor;
	AABBCollider* FloorCollider;
};

