#pragma once
#include <d3d12.h>
#include <stdint.h>
#include "Graphics/Color/Color.h"
#include "System/Singleton/Singleton.h"
#include "Math/Vector3/Vector3.h"
#include "Math/Matrix/Matrix.h"

class VertexBuffer;
enum class EIndexBufferFormat : uint8_t;
class IndexBuffer;
class ConstantBuffer;

namespace Graphics
{
	struct LineVertex;
	struct SimpleSpriteVertex;
	struct SpriteVertex;
	struct SimpleMeshVertex;
	struct StaticMeshVertex;
	struct SkeletalMeshVertex;

	/// <summary>
	/// 描画管理クラス
	/// </summary>
	class Renderer : public System::Singleton<Renderer>
	{
		GENERATE_SINGLETON_BODY(Renderer)

	public:
		/// <summary>
		/// インスタンス生成時の処理
		/// </summary>
		virtual void OnCreate()override;

		/// <summary>
		/// インスタンスの解放時の処理
		/// </summary>
		virtual void OnDestroy()override;

		/// <summary>
		/// 新規描画フレームの開始
		/// </summary>
		void NewRenderingFrame();

		/// <summary>
		/// 3D用メッシュの描画
		/// </summary>
		/// <param name="meshVertices">頂点配列の先頭アドレス</param>
		/// <param name="vertexCount">頂点配列数</param>
		/// <param name="meshIndices">インデックス配列の先頭アドレス</param>
		/// <param name="indexCount">インデックス配列数</param>
		void DrawMesh(
			const SkeletalMeshVertex* meshVertices, const size_t vertexCount,
			const uint32_t* meshIndices, const size_t indexCount);

		/// <summary>
		/// 2D用スプライトの描画
		/// </summary>
		/// <param name="spriteVertices">頂点配列の先頭アドレス</param>
		/// <param name="vertexCount">頂点配列数</param>
		/// <param name="spriteIndices">インデックス配列の先頭アドレス</param>
		/// <param name="indexCount">インデックス配列数</param>
		void DrawSprite(
			const SpriteVertex* spriteVertices, const size_t vertexCount,
			const uint16_t* spriteIndices, const size_t indexCount);

		/// <summary>
		/// ワールド空間へのライン描画
		/// </summary>
		/// <param name="lineVertices">頂点配列の先頭アドレス</param>
		/// <param name="vertexCount">頂点配列数</param>
		/// <param name="lineIndices">インデックス配列の先頭アドレス</param>
		/// <param name="indexCount">インデックス配列の先頭アドレス</param>
		void DrawLines(
			const LineVertex* lineVertices, const size_t vertexCount,
			const uint16_t* lineIndices, const size_t indexCount);

	private:

		/// <summary>
		/// 基本描画関数
		/// </summary>
		/// <param name="vertexData"></param>
		/// <param name="verteStride"></param>
		/// <param name="vertexCount"></param>
		/// <param name="indexData"></param>
		/// <param name="indexFormat"></param>
		/// <param name="indexCount"></param>
		void Draw(
			const void* vertexData,	const size_t verteStride, const size_t vertexCount,
			const void* indexData, const EIndexBufferFormat indexFormat, const size_t indexCount);

		struct CameraBuffer
		{
			Math::Matrix View;
			Math::Matrix Projection;
		};

		bool CreateShader();
		bool CreateRootSignature();
		bool CreatePipeline();
		D3D12_DEPTH_STENCIL_DESC GetDepthStencilDesc();
		D3D12_BLEND_DESC GetBlendStateDesc();
		D3D12_RASTERIZER_DESC GetRasterizerDesc();

		ID3D12RootSignature* RootSignature = nullptr;
		D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateDesc = {};
		ID3D12PipelineState* PipelineState = nullptr;
		D3D12_INPUT_ELEMENT_DESC InputLayout[2] = {};
		ID3DBlob* VertexShaderBlob = nullptr;
		ID3DBlob* PixelShaderBlob = nullptr;

		VertexBuffer* vertexBuffer = nullptr;

		IndexBuffer* indexBuffer = nullptr;

		ConstantBuffer* constantBuffer = nullptr;

		size_t VertexBufferOffset = 0;
		size_t IndexBufferOffset = 0;

		size_t UsingVertexBufferSize = 0;
		size_t UsingIndexBufferSize = 0;
	};
}