#include "Field.h"
#include "Graphics/FbxMesh/FbxMesh.h"
#include "../FrameWork/Math/Int2/Int2.h"
#include "../Source/Stage/GameTypes.h"
#include "../Source/Stage/Stage.h"
#include <Graphics/Color/Color.h>
#include <Math/Vector3/Vector3.h>

#define STAGE_RENDER


bool Field::Init(Stage* stage)
{
	for (int z = 0; z < GridH; z++)
	{
		for (int x = 0; x < GridW; x++)
		{
			const int index = z * GridW + x;

			Cells[index] = new FbxMesh();
			Cells[index]->Create("Assets/Cube/Cube.fbx.bin");

			// 色分け
			const Int2 grid(x, z);
			const CellType type = stage->GetCellType(grid);

			switch (type)
			{
			case CellType::Explosion:
				Cells[index]->SetColor(Color::Red);
				break;
			case CellType::Goal:
				Cells[index]->SetColor(Color::Green);
				break;
			case CellType::Wall:
				Cells[index]->SetColor(Color::Gray);
				break;
			default:
				Cells[index]->SetColor(Color::White);
				break;
			}

			// ワールド座標に変換
			Math::Vector3 pos = stage->GridToWorld(grid);
			pos.y = -1.0f;

			// セルのサイズ取得
			constexpr float cellSize = Stage::GetCellSize();
			Cells[index]->SetPosition(pos);
			Cells[index]->SetScale({ cellSize, 1.0f, cellSize });
		}
	}

#ifdef STAGE_RENDER
	stageMesh = new FbxMesh();
	stageMesh->Create("Assets/Stage/Stage.fbx.bin");

	// 座標・スケール調整
	stageMesh->SetPosition({ 0.0f, 0.0f, 0.0f });
	stageMesh->SetScale({ 0.1f, 0.1f, 0.1f });
	constexpr float DEG_TO_RAD = 3.1415926535f / 180.0f;
	Math::Quaternion y90 =
		Math::Quaternion::AngleAxis(270.0f * DEG_TO_RAD, Math::Vector3{ 1.0f, 0.0f, 0.0f });

	stageMesh->SetRotation(y90);
	stageMesh->SetColor(Color::White);
#endif

	return true;
}

void Field::Release() 
{
	for (auto& cell : Cells)
	{
		if (cell)
		{
			cell->Release();
			delete stageMesh;
			stageMesh = nullptr;
		}
	}

	if (stageMesh)
	{
		stageMesh->Release();
		delete stageMesh;
		stageMesh = nullptr;
	}
}
void Field::Update()
{

}
void Field::Render()
{
	for (auto& cell : Cells)
	{
		if (cell)
		{
			cell->Render();
		}
	}

#ifdef STAGE_RENDER
	if (stageMesh)
	{
		stageMesh->Render();
	}
#endif
}