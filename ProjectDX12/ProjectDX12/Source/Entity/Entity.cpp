#include "Entity.h"
#include "Graphics/FbxMesh/FbxMesh.h"
#include <Math/Vector3/Vector3.h>
#include <Math/Quaternion/Quaternion.h>

void Entity::Draw()
{
	if (!model) return;

	model->SetPosition(position);
	model->SetRotation(rotation);
	model->Render();
}

void Entity::Release()
{
	if (model)
	{
		model->Release();
		delete model;
		model = nullptr;
	}
}

void Entity::SetPosition(const Math::Vector3& pos)
{
	position = pos;
}

void Entity::SetRotation(const Math::Quaternion& rot)
{
	rotation = rot;
}