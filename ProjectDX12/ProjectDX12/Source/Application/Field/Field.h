#pragma once
#include <array>
#include "Graphics/FbxMesh/FbxMesh.h"

class ConstantBuffer;
class Stage;

class Field
{
public:
    bool Init(Stage* stage);
    void Update();
    void Render();
    void Release();

private:
    static constexpr int GridW = 9;
    static constexpr int GridH = 9;
    static constexpr int CellCount = GridW * GridH;

    /// <summary>
    /// セルごとの定数バッファデータ（事前計算済み）
    /// </summary>
    struct CellData
    {
        FbxMesh::MeshConstantBufferInfo bufferInfo;
    };

    // 共有メッシュ（1つだけ作成、全セルで共有）
    FbxMesh* cubeMesh = nullptr;

    // セルごとの定数バッファ（各自独立したGPUバッファ）
    std::array<ConstantBuffer*, CellCount> cellBuffers{};

    // セルごとの事前計算済みデータ
    std::array<CellData, CellCount> cellDataList{};

    // ステージのメッシュ
    FbxMesh* stageMesh = nullptr;
};
