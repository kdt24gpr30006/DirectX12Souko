#include "Field.h"
#include "Graphics/FbxMesh/FbxMesh.h"
#include "System/Collider/AABB/AABBCollider.h"
#include <random>

Field::Field()
	: Boxes()
	, BoxColliders()
	, Floor(nullptr)
	, FloorCollider(nullptr)
{
}
Field::~Field()
{
}

bool Field::Initialize()
{
	Floor = new FbxMesh();
	Floor->Create("Assets/Cube/Cube.fbx.bin");
	Floor->SetPosition(Math::Vector3(0.0f, -10.0f, 0.0f));
	Floor->SetScale(Math::Vector3(100.0f, 10.0f, 100.0f));
	Floor->SetColor(Color::Cyan);
	
	FloorCollider = new AABBCollider();
	FloorCollider->SetVolume(Math::Vector3(100.0f, 10.0f, 100.0f));
	const Math::Vector3 center = Floor->GetPosition() + Math::Vector3(0,5,0);
	FloorCollider->SetCenter(center);

#if false
	std::mt19937 rand = {};
	const int size = static_cast<int>(Boxes.size());
	for (int i = 0; i < size; i++)
	{
		Boxes[i] = new FbxMesh();
		Boxes[i]->Create("Assets/Cube/Cube.fbx.bin");
#if true
#else
		if (i % 2 == 0)
		{
			Boxes[i]->SetColor(Color::White);
		}
		else
		{
			Boxes[i]->SetColor(Color::Gray);
		}
#endif
		const float volume = 10.0f;
		Math::Vector3 position = {};
		position.x = volume * float(i % NumHalfSize);
		position.x -= volume * 0.5f * NumHalfSize;

		const auto randomValue = rand();
		if (randomValue % 4 == 0)
		{
			position.y = 0.0f;
			Boxes[i]->SetColor(Color::Gray);
		}
		else
		{
			position.y = -volume;
			Boxes[i]->SetColor(Color::White);
		}
		
		position.z = volume * float(i / NumHalfSize);
		position.z -= volume * 0.5f * NumHalfSize;

		Boxes[i]->SetScale(Math::Vector3(volume, volume, volume));
		Boxes[i]->SetPosition(position);

		BoxColliders[i] = new AABBCollider();
		const Math::Vector3 colliderSize = Math::Vector3(volume, volume, volume);
		BoxColliders[i]->SetVolume(colliderSize);
		const Math::Vector3 center = position + Math::Vector3(0.0f, volume * 0.5f, 0.0f);
		BoxColliders[i]->SetCenter(center);
	}
#endif
	return true;
}
void Field::Release() 
{
	for (auto& itr : Boxes)
	{
		if (itr != nullptr)
		{
			itr->Release();
			delete itr;
			itr = nullptr;
		}
	}
	for (auto& itr : BoxColliders)
	{
		if (itr != nullptr)
		{
			delete itr;
			itr = nullptr;
		}
	}

	if (Floor != nullptr)
	{
		Floor->Release();
		delete Floor;
		Floor = nullptr;
	}
	if (FloorCollider != nullptr)
	{
		delete FloorCollider;
		FloorCollider = nullptr;
	}
}
void Field::Update()
{

}
void Field::Render()
{
	if (Floor != nullptr)
	{
		Floor->Render();
	}
	if (FloorCollider != nullptr)
	{
		FloorCollider->DebugRender();
	}
	for (auto& itr : Boxes)
	{
		if (itr != nullptr)
		{
			itr->Render();
		}
	}
}

bool Field::CheckFieldCollider(const AABBCollider* other, Math::Vector3& outVector)const
{
	outVector.x = 0.0f;
	outVector.y = 0.0f;
	outVector.z = 0.0f;
	const int size = static_cast<int>(Boxes.size());
	for (const auto& itr : BoxColliders)
	{
		if (itr == nullptr)
		{
			continue;
		}
		const bool bHit = itr->CheckHit(other, outVector);
		if (bHit == true)
		{
			return true;
		}
	}
	return false;
}

bool Field::CheckFloorCollider(const AABBCollider* other, Math::Vector3& outVector)const
{
	if (FloorCollider == nullptr)
	{
		outVector.x = 0.0f;
		outVector.y = 0.0f;
		outVector.z = 0.0f;
		return false;
	}
	
	const bool bHit = FloorCollider->CheckHit(other, outVector);
	return bHit;
}