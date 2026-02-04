#pragma once
#include <d3d12.h>
#include "../FrameWork/Math/Vector2/Vector2.h"
#include "../FrameWork/Graphics/Texture/Texture.h"

namespace Graphics
{
    /// <summary>
    /// Sprite 用頂点構造体
    /// POSITION : スクリーン座標系（ピクセル）
    /// UV       : テクスチャ座標
    /// </summary>
    struct SpriteVertex;
}

/// <summary>
/// 2D スプライト描画クラス
/// ・Pipeline / RootSignature を内部で保持
/// ・Texture は外部管理（非所有）
/// </summary>
class Sprite
{
public:
    Sprite();
    ~Sprite();

    /// <summary>
    /// DX12 描画リソース生成
    /// </summary>
    bool Create();

    /// <summary>
    /// 描画に使用するテクスチャを設定（非所有）
    /// </summary>
    void SetTexture(Texture* texture);

    /// <summary>
    /// スプライト描画
    /// </summary>
    void Draw();

    // Transform
    void SetPosition(const Math::Vector2& pos);
    void SetPivot(const Math::Vector2& pivot);
    void SetScale(const Math::Vector2& scale);
    void SetSize(const Math::Vector2& size);
    void SetAngle(float angle);

    const Math::Vector2& GetPosition() const;
    const Math::Vector2& GetPivot() const;
    const Math::Vector2& GetScale() const;
    const Math::Vector2& GetSize() const;
    float GetAngle() const;

private:
    void UpdateVertices(Graphics::SpriteVertex* vertices);

    bool CreateShader();
    bool CreateRootSignature();
    bool CreatePipeline();

private:
    ID3D12RootSignature* RootSignature = nullptr;
    ID3D12PipelineState* PipelineState = nullptr;

    ID3DBlob* VSBlob = nullptr;
    ID3DBlob* PSBlob = nullptr;

    Texture* Tex = nullptr;  

    Math::Vector2 Position{ 0,0 };
    Math::Vector2 Pivot{ 0,0 };
    Math::Vector2 Scale{ 1,1 };
    Math::Vector2 Size{ 256,256 };
    float Angle = 0.0f;
};
