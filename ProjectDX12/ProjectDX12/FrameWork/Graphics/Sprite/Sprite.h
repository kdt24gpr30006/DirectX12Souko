#pragma once
#include <d3d12.h>
#include <filesystem>
#include "../../Math/Vector2/Vector2.h"
#include "../../Math/Vector3/Vector3.h"

class GraphicsDescriptorHeap;
class Texture;

namespace Graphics
{
	struct SpriteVertex;
}

/// <summary>
/// 2Dスプライト描画クラス
/// </summary>
class Sprite
{
public:
	Sprite();
	~Sprite();

	/// <summary>
	/// スプライトの作成
	/// </summary>
	/// <returns>true:成功</returns>
	bool Create();

	/// <summary>
	/// スプライトの作成
	/// </summary>
	/// <param name="FilePath">テクスチャパス</param>
	/// <returns>true:成功</returns>
	bool Create(const std::filesystem::path& FilePath);

	/// <summary>
	/// 描画
	/// </summary>
	void Render();

	/// <summary>
	/// テクスチャ描画
	/// </summary>
	void RenderTexture();

	/// <summary>
	/// 座標の取得
	/// </summary>
	/// <returns>座標</returns>
	const Math::Vector2& GetPosition()const;
	/// <summary>
	/// 座標の指定
	/// </summary>
	/// <param name="position">座標</param>
	void SetPosition(const Math::Vector2& position);

	/// <summary>
	/// 軸の取得
	/// </summary>
	/// <returns>正規化された軸の座標</returns>
	const Math::Vector2& GetPivot()const;
	/// <summary>
	/// 中心となる軸の指定
	/// 座標は正規化された値を指定する
	/// </summary>
	/// <param name="pivot">軸 (X:0 ~ 1.0, Y:0 ~ 1.0f)</param>
	void SetPivot(const Math::Vector2& pivot);

	/// <summary>
	/// 拡縮の取得
	/// </summary>
	/// <returns>拡縮の倍率</returns>
	const Math::Vector2& GetScale()const;
	/// <summary>
	/// 拡縮の倍率の指定
	/// </summary>
	/// <param name="scale">拡縮の倍率</param>
	void SetScale(const Math::Vector2& scale);

	/// <summary>
	/// サイズの取得
	/// </summary>
	/// <returns>サイズ(幅, 高さ)</returns>
	const Math::Vector2& GetSize()const;
	/// <summary>
	/// サイズの指定
	/// </summary>
	/// <param name="size">サイズ(幅, 高さ)</param>
	void SetSize(const Math::Vector2& size);

	/// <summary>
	/// 回転の角度の取得
	/// </summary>
	/// <returns>回転の角度</returns>
	float GetAngle()const;
	/// <summary>
	/// 回転の角度の指定
	/// </summary>
	/// <param name="angle">回転の角度(0 ~ 360)</param>
	void SetAngle(float angle);

private:
	/// <summary>
	/// 頂点座標変換
	/// </summary>
	void UpdateVertices(Graphics::SpriteVertex* Vertices);

	/// <summary>
	/// シェーダーの作成
	/// </summary>
	/// <returns>true:成功</returns>
	bool CreateShader();
	bool CreateTextureShader();

	/// <summary>
	/// ルートシグネチャの作成
	/// </summary>
	/// <returns>true:成功</returns>
	bool CreateRootSignature();
	bool CreateTextureRootSignature();
	
	/// <summary>
	/// 描画パイプラインの作成
	/// </summary>
	/// <returns>true:成功</returns>
	bool CreatePipeline();
	bool CreateTexturePipeline();

	/// <summary>
	/// 深度ステンシルの初期設定の取得
	/// </summary>
	/// <returns></returns>
	D3D12_DEPTH_STENCIL_DESC GetDepthStencilDesc();

	/// <summary>
	/// ブレンドステートの初期設定の取得
	/// </summary>
	/// <returns></returns>
	D3D12_BLEND_DESC GetBlendStateDesc();

	/// <summary>
	/// ラスタライザーの初期設定の取得
	/// </summary>
	/// <returns></returns>
	D3D12_RASTERIZER_DESC GetRasterizerDesc();

private:

	/// <summary>
	/// ルートシグネチャ
	/// </summary>
	ID3D12RootSignature* RootSignature;

	/// <summary>
	/// 描画パイプライン設定用
	/// </summary>
	D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateDesc;

	/// <summary>
	/// パイプラインステート
	/// </summary>
	ID3D12PipelineState* PipelineState;
	
	/// <summary>
	/// 頂点シェーダー情報
	/// </summary>
	ID3DBlob* VertexShaderBlob;
	/// <summary>
	/// 頂点シェーダー用インプットレイアウト
	/// </summary>
	D3D12_INPUT_ELEMENT_DESC InputLayout[2];

	/// <summary>
	/// ピクセルシェーダー情報
	/// </summary>
	ID3DBlob* PixelShaderBlob;

	/// <summary>
	/// テクスチャ
	/// </summary>
	Texture* Tex;

	/// <summary>
	/// 座標
	/// </summary>
	Math::Vector2 Position;

	/// <summary>
	/// 中心となる軸
	/// </summary>
	Math::Vector2 Pivot;

	/// <summary>
	/// 拡縮
	/// </summary>
	Math::Vector2 Scale;

	/// <summary>
	/// サイズ(幅,高さ)
	/// </summary>
	Math::Vector2 Size;

	/// <summary>
	/// 回転の角度
	/// </summary>
	float Angle;

};

