#pragma once
#include <vector>
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
    int gridW = 0;
    int gridH = 0;
    int cellCount = 0;

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
    std::vector<ConstantBuffer*> cellBuffers;

    // セルごとの事前計算済みデータ
    std::vector<CellData> cellDataList;

    // ステージのメッシュ
    FbxMesh* stageMesh = nullptr;
};
