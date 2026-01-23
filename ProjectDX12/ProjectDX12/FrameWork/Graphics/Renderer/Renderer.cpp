#include "Renderer.h"
#include <d3dcompiler.h>
#include "Graphics/DirectX/DirectX.h"
#include "Graphics/VertexBuffer/VertexBuffer.h"
#include "Graphics/IndexBuffer/IndexBuffer.h"
#include "Graphics/ConstantBuffer/ConstantBuffer.h"
#include "Graphics/Graphics.h"
#include "System/Camera/Camera.h"
#include "System/Debug/Debug.h"

namespace Graphics
{
	/// <summary>
	/// インスタンス生成時の処理
	/// </summary>
	void Renderer::OnCreate()
	{
		bool bSuccess = false;


		vertexBuffer = new VertexBuffer();
		bSuccess = vertexBuffer->Create(
			PerMaxVertexBufferSize * 2, sizeof(LineVertex));
		if (bSuccess == false)
		{
			DEBUG_LOG(ELogLevel::Fatal,
				"Failed Create RendererVertexBuffer.\n");
		}

		indexBuffer = new IndexBuffer();
		bSuccess = indexBuffer->Create(
			PerMaxIndexBufferSize * 2);
		if (bSuccess == false)
		{
			DEBUG_LOG(ELogLevel::Fatal,
				"Failed Create RendererIndexBuffer.\n");
		}

		constantBuffer = new ConstantBuffer();
		bSuccess = constantBuffer->Create(sizeof(CameraBuffer));



		bSuccess = CreateShader();
		bSuccess = CreateRootSignature();
		bSuccess = CreatePipeline();
	}

	/// <summary>
	/// インスタンスの解放時の処理
	/// </summary>
	void Renderer::OnDestroy()
	{
		SafeRelease(RootSignature);
		SafeRelease(PipelineState);
		if (vertexBuffer != nullptr)
		{
			vertexBuffer->Release();
			delete vertexBuffer;
			vertexBuffer = nullptr;
		}
		if (indexBuffer != nullptr)
		{
			indexBuffer->Release();
			delete indexBuffer;
			indexBuffer = nullptr;
		}
		if (constantBuffer != nullptr)
		{
			constantBuffer->Release();
			delete constantBuffer;
			constantBuffer = nullptr;
		}
	}

	/// <summary>
	/// 新規描画フレームの開始
	/// </summary>
	void Renderer::NewRenderingFrame()
	{
		Graphics::DirectX* instance = Graphics::DirectX::GetInstance();
		const size_t FrameIndex = static_cast<size_t>(instance->GetFrameIndex());
		
		VertexBufferOffset = PerMaxVertexBufferSize * FrameIndex;
		IndexBufferOffset = PerMaxIndexBufferSize * FrameIndex;

		UsingVertexBufferSize = 0;
		UsingIndexBufferSize = 0;
	}

	/// <summary>
	/// 3D用メッシュの描画
	/// </summary>
	/// <param name="meshVertices">頂点配列の先頭アドレス</param>
	/// <param name="vertexCount">頂点配列数</param>
	/// <param name="meshIndices">インデックス配列の先頭アドレス</param>
	/// <param name="indexCount">インデックス配列数</param>
	void Renderer::DrawMesh(const SkeletalMeshVertex* meshVertices, const size_t vertexCount, const uint32_t* meshIndices, const size_t indexCount)
	{
		Draw(
			meshVertices, sizeof(SkeletalMeshVertex), vertexCount,
			meshIndices, EIndexBufferFormat::Uint32, indexCount);
	}

	/// <summary>
	/// 2D用スプライトの描画
	/// </summary>
	/// <param name="spriteVertices">頂点配列の先頭アドレス</param>
	/// <param name="vertexCount">頂点配列数</param>
	/// <param name="lineIndices">インデックス配列の先頭アドレス</param>
	/// <param name="indexCount">インデックス配列数</param>
	void Renderer::DrawSprite(
		const SpriteVertex* spriteVertices, 
		const size_t vertexCount, 
		const uint16_t* lineIndices,
		const size_t indexCount)
	{
		Draw(
			spriteVertices, sizeof(SpriteVertex), vertexCount,
			lineIndices, EIndexBufferFormat::Uint16, indexCount);
	}

	/// <summary>
	/// ワールド空間へのライン描画
	/// </summary>
	/// <param name="lineVertices">頂点配列の先頭アドレス</param>
	/// <param name="vertexCount">頂点配列数</param>
	/// <param name="lineIndices">インデックス配列の先頭アドレス</param>
	/// <param name="indexCount">インデックス配列の先頭アドレス</param>
	void Renderer::DrawLines(
		const LineVertex* lineVertices,
		const size_t vertexCount,
		const uint16_t* lineIndices,
		const size_t indexCount)
	{
		Graphics::DirectX* instance = Graphics::DirectX::GetInstance();
		ID3D12GraphicsCommandList* commandList = instance->GetCommandList();
		commandList->SetGraphicsRootSignature(RootSignature);
		commandList->SetPipelineState(PipelineState);
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

		//	カメラ用バッファ更新
		const CameraBuffer cameraBuffer =
		{
			Math::Matrix::Transpose(Camera::Main->GetView()),
			Math::Matrix::Transpose(Camera::Main->GetProjection())
		};
		constantBuffer->Update(&cameraBuffer);
		constantBuffer->Set(0);

		Draw(
			lineVertices, sizeof(LineVertex), vertexCount,
			lineIndices, EIndexBufferFormat::Uint16, indexCount);
	}

	/// <summary>
	/// 基本描画関数
	/// </summary>
	/// <param name="vertexData"></param>
	/// <param name="verteStride"></param>
	/// <param name="vertexCount"></param>
	/// <param name="indexData"></param>
	/// <param name="indexFormat"></param>
	/// <param name="indexCount"></param>
	void Renderer::Draw(
		const void* vertexData, const size_t verteStride, const size_t vertexCount,
		const void* indexData, const EIndexBufferFormat indexFormat, const size_t indexCount)
	{
		const size_t vertexSize = verteStride * vertexCount;
		if (UsingVertexBufferSize + vertexSize > PerMaxVertexBufferSize)
		{
			//	1フレームに使える頂点バッファが足りなくなった
			DEBUG_LOG(ELogLevel::Fatal, "Not Enough Renderer VertexBuffer.\n");
		}
		vertexBuffer->Update(vertexData, VertexBufferOffset, vertexSize);
		//	頂点バッファのセット
		vertexBuffer->Set(VertexBufferOffset, verteStride, vertexSize);

		size_t IndexStride = 0;
		switch (indexFormat)
		{
		case EIndexBufferFormat::Uint32:
			IndexStride = sizeof(uint32_t);
			break;
		case EIndexBufferFormat::Uint16:
			IndexStride = sizeof(uint16_t);
			break;
		default:
			break;
		}
		const size_t indexSize = IndexStride * indexCount;
		if (UsingIndexBufferSize + indexSize > PerMaxIndexBufferSize)
		{
			//	1フレームに使えるインデックスバッファが足りなくなった
			DEBUG_LOG(ELogLevel::Fatal, "Not Enough Renderer IndexBuffer.\n");
		}

		indexBuffer->Update(indexData, IndexBufferOffset, indexSize);
		indexBuffer->Set(indexFormat, IndexBufferOffset, indexSize);

		Graphics::DirectX* instance = Graphics::DirectX::GetInstance();
		ID3D12GraphicsCommandList* commandList = instance->GetCommandList();
		//各バッファのセット時にバッファの先頭位置をずらしているのでほとんど0指定でOK
		commandList->DrawIndexedInstanced(
			static_cast<UINT>(indexCount), 1, 0, 0, 0);

		UsingVertexBufferSize += vertexSize;
		UsingIndexBufferSize += indexSize;

		VertexBufferOffset += vertexSize;
		IndexBufferOffset += indexSize;
	}

	bool Renderer::CreateShader()
	{
		//	シェーダーコンパイル用処理
		auto CompileShader = [](const char* FileName, const char* FunctionName, const char* ShaderModel)
			{
				size_t wLen = 0;
				wchar_t wideFileName[256] = {};
				//	ファイル名変換
				mbstowcs_s(&wLen, wideFileName, ARRAYSIZE(wideFileName), FileName, _TRUNCATE);

				//	シェーダー作成
				ID3DBlob* ShaderBlob = nullptr;
				ID3DBlob* ErrorBlob = nullptr;

				const HRESULT hr = D3DCompileFromFile(wideFileName,
					nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
					FunctionName, ShaderModel,
					D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
					0, &ShaderBlob, &ErrorBlob);

				if (FAILED(hr))
				{
					DEBUG_LOG(ELogLevel::Fatal, (char*)ErrorBlob->GetBufferPointer());
					SafeRelease(ErrorBlob);
				}

				return ShaderBlob;
			};

		VertexShaderBlob = CompileShader("Assets/Shader/Line.hlsl", "VertexMain", "vs_5_0");
		if (VertexShaderBlob == nullptr)
		{
			return false;
		}
		//	頂点シェーダーへの入力情報の設定
		for (auto& IL : InputLayout)
		{
			IL.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
			IL.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			IL.InstanceDataStepRate = 0;
			IL.SemanticIndex = 0;
			IL.InputSlot = 0;
		}
		InputLayout[0].SemanticName = "POSITION";
		InputLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;//float3

		InputLayout[1].SemanticName = "COLOR";
		InputLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;//float4

		PixelShaderBlob = CompileShader("Assets/Shader/Line.hlsl", "PixelMain", "ps_5_0");
		if (PixelShaderBlob == nullptr)
		{
			return false;
		}
		return true;
	}
	bool Renderer::CreateRootSignature()
	{
		D3D12_DESCRIPTOR_RANGE Range = {};
		Range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		Range.NumDescriptors = 1;
		Range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		Range.BaseShaderRegister = 0;
		Range.RegisterSpace = 0;

		//	ルートパラメーター設定
		D3D12_ROOT_PARAMETER RootParam = {};
		RootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		RootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		RootParam.DescriptorTable.pDescriptorRanges = &Range;
		RootParam.DescriptorTable.NumDescriptorRanges = 1;

		D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc = {};
		RootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		RootSignatureDesc.pParameters = &RootParam;
		RootSignatureDesc.NumParameters = 1;

		//	ルートシグネチャ設定
		ID3DBlob* RootSignatureBlob = nullptr;
		ID3DBlob* ErrorBlob = nullptr;
		HRESULT hr = D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &RootSignatureBlob, &ErrorBlob);
		if (FAILED(hr))
		{
			DEBUG_LOG(ELogLevel::Fatal, "Failed SerializeRootSignature.\n");
			SafeRelease(ErrorBlob);
			return false;
		}

		ID3D12Device* Device = Graphics::DirectX::GetInstance()->GetDevice();
		hr = Device->CreateRootSignature(0, RootSignatureBlob->GetBufferPointer(), RootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&RootSignature));
		SafeRelease(RootSignatureBlob);
		if (FAILED(hr))
		{
			DEBUG_LOG(ELogLevel::Fatal, "Failed CreateRootSignature.\n");
			return false;
		}

		return true;
	}
	bool Renderer::CreatePipeline()
	{
		PipelineStateDesc.pRootSignature = RootSignature;

		//コンパイルした頂点シェーダーの情報を格納
		PipelineStateDesc.VS.pShaderBytecode = VertexShaderBlob->GetBufferPointer();
		PipelineStateDesc.VS.BytecodeLength = VertexShaderBlob->GetBufferSize();
		PipelineStateDesc.InputLayout.NumElements = ARRAYSIZE(InputLayout);
		PipelineStateDesc.InputLayout.pInputElementDescs = InputLayout;

		//コンパイルしたピクセルシェーダーの情報を格納
		PipelineStateDesc.PS.pShaderBytecode = PixelShaderBlob->GetBufferPointer();
		PipelineStateDesc.PS.BytecodeLength = PixelShaderBlob->GetBufferSize();

		ID3D12Device* Device = Graphics::DirectX::GetInstance()->GetDevice();
		HRESULT hr = 0;

		//	ブレンドステート設定
		PipelineStateDesc.BlendState = GetBlendStateDesc();
		//	ラスタライザー設定
		PipelineStateDesc.RasterizerState = GetRasterizerDesc();
		//	深度ステンシル設定
		PipelineStateDesc.DepthStencilState = GetDepthStencilDesc();

		PipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		PipelineStateDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

		//	描画先のレンダーターゲットの設定
		PipelineStateDesc.NumRenderTargets = 1;
		PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		PipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		PipelineStateDesc.SampleDesc.Count = 1;
		PipelineStateDesc.SampleDesc.Quality = 0;

		//	パイプライン作成
		hr = Device->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&PipelineState));

		if (FAILED(hr))
		{
			DEBUG_LOG(ELogLevel::Fatal, "Failed CreateGraphicsPipelineState.\n");
			return false;
		}
		return true;
	}
	D3D12_DEPTH_STENCIL_DESC Renderer::GetDepthStencilDesc()
	{
		D3D12_DEPTH_STENCIL_DESC Desc = {};
		Desc.DepthEnable = TRUE;
		Desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		Desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		Desc.StencilEnable = FALSE;
		Desc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
		Desc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

		Desc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		Desc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		Desc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		Desc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		Desc.BackFace = Desc.FrontFace;

		return Desc;
	}
	D3D12_BLEND_DESC Renderer::GetBlendStateDesc()
	{
		D3D12_BLEND_DESC Desc = {};
		Desc.AlphaToCoverageEnable = FALSE;
		Desc.IndependentBlendEnable = FALSE;
		D3D12_RENDER_TARGET_BLEND_DESC RenderTargetBlendDesc = {};
		RenderTargetBlendDesc.BlendEnable = FALSE;
		RenderTargetBlendDesc.LogicOpEnable = FALSE;
		RenderTargetBlendDesc.SrcBlend = D3D12_BLEND_ONE;
		RenderTargetBlendDesc.DestBlend = D3D12_BLEND_ZERO;
		RenderTargetBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		RenderTargetBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		RenderTargetBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
		RenderTargetBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		RenderTargetBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
		RenderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		for (auto& rt : Desc.RenderTarget)
		{
			rt = RenderTargetBlendDesc;
		}
		return Desc;
	}
	D3D12_RASTERIZER_DESC Renderer::GetRasterizerDesc()
	{
		D3D12_RASTERIZER_DESC Desc = {};
		Desc.FillMode = D3D12_FILL_MODE_WIREFRAME;
		Desc.CullMode = D3D12_CULL_MODE_BACK;
		Desc.FrontCounterClockwise = FALSE;
		Desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		Desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		Desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		Desc.DepthClipEnable = FALSE;
		Desc.MultisampleEnable = FALSE;
		Desc.AntialiasedLineEnable = FALSE;
		Desc.ForcedSampleCount = 0;
		Desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		return Desc;
	}
}