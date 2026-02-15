#include "Field.h"
#include "Graphics/FbxMesh/FbxMesh.h"
#include "Graphics/ConstantBuffer/ConstantBuffer.h"
#include "../FrameWork/Math/Int2/Int2.h"
#include "../Source/Stage/GameTypes.h"
#include "../Source/Stage/Stage.h"
#include <Graphics/Color/Color.h>
#include <Math/Vector3/Vector3.h>
#include <Math/Matrix/Matrix.h>
#include <Math/Quaternion/Quaternion.h>
#include "System/Camera/Camera.h"

//#define STAGE_RENDER

bool Field::Init(Stage* stage)
{
	// Stageからグリッドサイズを取得
	gridW = stage->GetGridWidth();
	gridH = stage->GetGridHeight();
	cellCount = gridW * gridH;

	// 共有メッシュを1つだけ作成（シェーダーコンパイル・パイプライン作成は1回のみ）
	cubeMesh = new FbxMesh();
	if (!cubeMesh->Create("Assets/Cube/Cube.fbx.bin"))
	{
		delete cubeMesh;
		cubeMesh = nullptr;
		return false;
	}

	// バッファを動的に確保
	cellBuffers.resize(cellCount, nullptr);
	cellDataList.resize(cellCount);

	// 定数バッファを作成（GPU バッファ＋デスクリプタのみ、シェーダーコンパイル不要）
	for (int z = 0; z < gridH; z++)
	{
		for (int x = 0; x < gridW; x++)
		{
			const int index = z * gridW + x;

			// セルタイプに応じた色を決定
			const Int2 grid(x, z);
			const CellType type = stage->GetCellType(grid);

			// None セルは描画しない
			if (type == CellType::None)
				continue;

			Color cellColor = Color::White;
			switch (type)
			{
			case CellType::Explosion:
				cellColor = Color::Red;
				break;
			case CellType::Goal:
				cellColor = Color::Green;
				break;
			case CellType::Wall:
				cellColor = Color::Gray;
				break;
			default:
				cellColor = Color::White;
				break;
			}

			// ワールド座標とスケールを計算
			Math::Vector3 pos = stage->GridToWorld(grid);
			constexpr float cellSize = Stage::GetCellSize();

			Math::Vector3 scale;
			if (type == CellType::Wall)
			{
				pos.y = 0.0f;
				scale = { cellSize, cellSize, cellSize };
			}
			else
			{
				pos.y = -1.0f;
				scale = { cellSize, 1.0f, cellSize };
			}

			// Transform 行列を事前計算
			CellData& data = cellDataList[index];
			data.bufferInfo.Transform.Update(
				pos,
				Math::Quaternion::Identity,
				scale
			);
			data.bufferInfo.Transform = Math::Matrix::Transpose(data.bufferInfo.Transform);
			data.bufferInfo.MeshColor = cellColor;
			data.bufferInfo.View = Math::Matrix::identity;
			data.bufferInfo.Projection = Math::Matrix::identity;

			// 定数バッファ作成（GPUバッファ＋CBVデスクリプタのみ）
			cellBuffers[index] = new ConstantBuffer();
			cellBuffers[index]->Create(FbxMesh::MeshConstantBufferSize);
			cellBuffers[index]->Update(&data.bufferInfo);
		}
	}

#ifdef STAGE_RENDER
	stageMesh = new FbxMesh();
	stageMesh->Create("Assets/Stage/Stage.fbx.bin");
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
	for (auto& cb : cellBuffers)
	{
		if (cb)
		{
			cb->Release();
			delete cb;
			cb = nullptr;
		}
	}
	cellBuffers.clear();

	if (cubeMesh)
	{
		cubeMesh->Release();
		delete cubeMesh;
		cubeMesh = nullptr;
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
	if (!cubeMesh || !Camera::Main)
		return;

	// カメラ行列はフレームごとに1回だけ取得（全セル共通）
	Math::Matrix view = Math::Matrix::Transpose(Camera::Main->GetView());
	Math::Matrix proj = Math::Matrix::Transpose(Camera::Main->GetProjection());

	// 定数バッファをカメラ行列で更新
	for (int i = 0; i < cellCount; i++)
	{
		if (!cellBuffers[i])
			continue;

		cellDataList[i].bufferInfo.View = view;
		cellDataList[i].bufferInfo.Projection = proj;
		cellBuffers[i]->Update(&cellDataList[i].bufferInfo);
	}

	// パイプラインステートを1回だけセット
	cubeMesh->BeginSharedRender();

	// 各セルを独自の定数バッファで描画
	for (int i = 0; i < cellCount; i++)
	{
		if (!cellBuffers[i])
			continue;

		cubeMesh->DrawWithExternalBuffer(cellBuffers[i]);
	}

#ifdef STAGE_RENDER
	if (stageMesh)
	{
		stageMesh->Render();
	}
#endif
}
