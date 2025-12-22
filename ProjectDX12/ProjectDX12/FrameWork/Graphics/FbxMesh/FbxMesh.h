#pragma once
#include <d3d12.h>
#include <filesystem>
#include <vector>
#include <string>
#include <unordered_map>
#include <array>
#include "Graphics/Color/Color.h"
#include "Math/Vector2/Vector2.h"
#include "Math/Vector3/Vector3.h"
#include "Math/Matrix/Matrix.h"
#include "Math/Quaternion/Quaternion.h"

namespace Graphics
{
	struct SkeletalMeshVertex;
}

class ConstantBuffer;
class GraphicsDescriptorHeap;
class Texture;

/// <summary>
/// Fbx描画用クラス
/// </summary>
class FbxMesh
{
public:
	FbxMesh();
	~FbxMesh();

	/// <summary>
	/// コンバート済みFbxモデルファイルの読み込み
	/// </summary>
	/// <param name="FilePath">ファイルパス XXX.fbx.bin</param>
	/// <returns>true:成功</returns>
	bool Create(const std::filesystem::path& FilePath);

	/// <summary>
	/// コンバート済みアニメーション(モーション)ファイルの読み込み
	/// </summary>
	/// <param name="AnimationName">アニメーション名</param>
	/// <param name="AnimationPath">ファイルパス XXX.fbx.anm</param>
	/// <returns>true:成功</returns>
	bool LoadAnimation(const std::string& AnimationName, const std::filesystem::path& AnimationPath);

	void Release();

	void Render();

	/// <summary>
	/// アニメーション用更新処理(フレームを進める)
	/// </summary>
	/// <param name="AnimationName">Load時に登録したアニメーション名</param>
	/// <param name="deltaTime"></param>
	/// <param name="bIsLoop">アニメーションをループさせるか</param>
	void Animate(
		const std::string& AnimationName,
		const float deltaTime,
		const bool bIsLoop);

	/// <summary>
	/// アニメーションの現在のフレームを0~1.0fの割合で取得する
	/// </summary>
	/// <returns>0~1.0f</returns>
	float GetCurrentAnimationFrameRate()const;

	const Math::Vector3& GetPosition()const;
	void SetPosition(const Math::Vector3& position);

	const Math::Quaternion& GetRotation()const;
	void SetRotation(const Math::Quaternion& rotaion);

	const Math::Vector3& GetScale()const;
	void SetScale(const Math::Vector3& scale);

	void SetColor(const Color& color);

private:
	/// <summary>
	/// コンバート済みfbxモデルデータの読み込み
	/// </summary>
	/// <param name="FilePath"></param>
	/// <returns></returns>
	bool LoadModelData(const std::string& FilePath);

	/// <summary>
	/// コンバート済みfbxアニメーションデータの読み込み
	/// </summary>
	/// <param name="AnimationName"></param>
	/// <param name="FilePath"></param>
	/// <returns></returns>
	bool LoadAnimationData(const std::string& AnimationName, const std::string& FilePath);

	/// <summary>
	/// スキニング処理
	/// </summary>
	/// <param name="AnimationName"></param>
	void Skinning(const std::string& AnimationName);

	bool CreateShader();

	bool CreateRootSignature();

	bool CreatePipeline();

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
	/// モデル用定数バッファ構造体
	/// </summary>
	struct MeshConstantBufferInfo
	{
		Math::Matrix Transform;
		Math::Matrix View;
		Math::Matrix Projection;
		Color MeshColor;
	};
	/// <summary>
	/// バッファサイズ
	/// </summary>
	static constexpr auto MeshConstantBufferSize = sizeof(MeshConstantBufferInfo);

	/// <summary>
	/// マテリアル用構造体
	/// </summary>
	struct Material
	{
		std::string Name;
		Texture* Tex;
		int PolygonCount;
		std::vector<Graphics::SkeletalMeshVertex> Vertices;
		std::vector<uint32_t> Indices;
	};

	/// <summary>
	/// ボーン用構造体
	/// </summary>
	struct BoneInfo
	{
		/// <summary>
		/// ボーン名
		/// </summary>
		std::string Name;

		/// <summary>
		/// 親のボーンのインデックス
		/// </summary>
		int ParentIndex;

		/// <summary>
		/// 現在のボーンの姿勢行列
		/// </summary>
		Math::Matrix Transform;

		/// <summary>
		/// 基本姿勢のボーン
		/// </summary>
		Math::Matrix BindMatrix;
	};

	/// <summary>
	/// アニメーション用構造体
	/// </summary>
	struct Animation
	{
		int NumFrame;
		std::vector<std::vector<Math::Matrix>> KeyFrames;
	};

	/// <summary>
	/// 頂点情報配列
	/// </summary>
	std::vector<Graphics::SkeletalMeshVertex> Vertices;

	/// <summary>
	/// インデックス情報配列
	/// </summary>
	std::vector<uint32_t> Indices;

	/// <summary>
	/// マテリアル系情報配列
	/// </summary>
	std::vector<Material> Materials;
	std::vector<Material> MaterialsSrc;

	/// <summary>
	/// 頂点変換用定数バッファ
	/// </summary>
	MeshConstantBufferInfo MeshBufferInfo;

	/// <summary>
	/// ボーン情報配列
	/// </summary>
	std::vector<BoneInfo> Bones;

	/// <summary>
	/// 読み込んだアニメーション列挙用
	/// </summary>
	std::unordered_map<std::string, Animation> AnimationList;

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
	std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayouts;

	/// <summary>
	/// ピクセルシェーダー情報
	/// </summary>
	ID3DBlob* PixelShaderBlob;

	ConstantBuffer* TransformBuffer;

	Math::Vector3 Position;

	Math::Quaternion Rotation;

	Math::Vector3 Scale;

	Color MeshColor;

	float Frame;

	std::string CurrentAnimationName;

	bool bIsAnimationFinish;
};

