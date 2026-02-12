#pragma once
#include <d3d12.h>
#include <d3dcommon.h>
#include "Graphics/Color/Color.h"
#include "Math/Matrix/Matrix.h"

class Texture;
class ConstantBuffer;

namespace Math { struct Vector3; }

/// <summary>
/// 足元の丸い影（ブロブシャドウ）描画クラス
/// 3Dワールド空間にXZ平面のクアッドとして描画する
/// </summary>
class BlobShadow
{
public:
    BlobShadow();
    ~BlobShadow();

    /// <summary>
    /// DX12描画リソース生成
    /// </summary>
    bool Create();

    /// <summary>
    /// 影テクスチャを設定（非所有）
    /// </summary>
    void SetTexture(Texture* texture);

    /// <summary>
    /// 指定ワールド座標に影を描画
    /// </summary>
    /// <param name="worldPos">影の中心座標</param>
    /// <param name="scale">影のスケール</param>
    void Draw(const Math::Vector3& worldPos, float scale);

    /// <summary>
    /// リソース解放
    /// </summary>
    void Release();

private:
    bool CreateShader();
    bool CreateRootSignature();
    bool CreatePipeline();

    struct ShadowConstantBuffer
    {
        Math::Matrix Transform;
        Math::Matrix View;
        Math::Matrix Projection;
        Color ShadowColor;
    };

    ID3D12RootSignature* RootSignature = nullptr;
    ID3D12PipelineState* PipelineState = nullptr;
    ID3DBlob* VSBlob = nullptr;
    ID3DBlob* PSBlob = nullptr;

    ConstantBuffer* TransformBuffer = nullptr;
    ShadowConstantBuffer BufferInfo{};

    Texture* Tex = nullptr;

    Color ShadowColor{ 0.0f, 0.0f, 0.0f, 0.5f };
};
