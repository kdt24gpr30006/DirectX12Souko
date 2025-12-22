#include "Sprite.h"
#include <d3dcompiler.h>
#include "Graphics/DirectX/DirectX.h"
#include "Graphics/VertexBuffer/VertexBuffer.h"
#include "Graphics/GraphicsDescriptorHeap/GraphicsDescriptorHeap.h"
#include "Graphics/Texture/Texture.h"
#include "Graphics/Renderer/Renderer.h"
#include "Graphics/Graphics.h"
#include "Math/Math.h"
#include "System/Window/Window.h"

#pragma comment(lib,"d3dcompiler.lib")

Sprite::Sprite()
	: RootSignature(nullptr)
	, PipelineStateDesc()
	, PipelineState(nullptr)
	, VertexShaderBlob(nullptr)
	, InputLayout()
	, PixelShaderBlob(nullptr)
	, Tex(nullptr)
	, Position(0.0f,0.0f)
	, Pivot(0.0f,0.0f)
	, Scale(1.0f,1.0f)
	, Size(256.0f,256.0f)
	, Angle(0.0f)
{
}
Sprite::~Sprite()
{
	SafeRelease(VertexShaderBlob);
	SafeRelease(PixelShaderBlob);
	SafeRelease(RootSignature);
	SafeRelease(PipelineState);
	if (Tex != nullptr)
	{
		Tex->Release();
		delete Tex;
		Tex = nullptr;
	}
}

/// <summary>
/// スプライトの作成
/// </summary>
/// <returns>true:成功</returns>
bool Sprite::Create()
{
	//シェーダーの作成
	bool ret = CreateShader();
	if (ret == false)
	{
		return false;
	}
	//ルートシグネチャの作成
	ret = CreateRootSignature();
	if (ret == false)
	{
		return false;
	}
	//パイプラインの作成
	ret = CreatePipeline();
	if (ret == false)
	{
		return false;
	}

	return true;
}

/// <summary>
/// スプライトの作成
/// </summary>
/// <param name="FilePath">テクスチャパス</param>
/// <returns>true:成功</returns>
bool Sprite::Create(const std::filesystem::path& FilePath)
{
	Tex = new Texture();
	bool ret = Tex->Create(FilePath);
	if (ret == false)
	{
		return false;
	}
	//	読み込んだ画像のサイズを一旦標準にする
	Size.x = Tex->GetWidth();
	Size.y = Tex->GetHeight();

	//シェーダーの作成
	ret = CreateTextureShader();
	if (ret == false)
	{
		return false;
	}
	//ルートシグネチャの作成
	ret = CreateTextureRootSignature();
	if (ret == false)
	{
		return false;
	}
	//パイプラインの作成
	ret = CreateTexturePipeline();
	if (ret == false)
	{
		return false;
	}

	return true;
}

/// <summary>
/// 描画
/// </summary>
void Sprite::Render()
{
	Graphics::DirectX* DirextXInstance = Graphics::DirectX::GetInstance();
	ID3D12GraphicsCommandList* CommandList = DirextXInstance->GetCommandList();

	CommandList->SetGraphicsRootSignature(RootSignature);
	CommandList->SetPipelineState(PipelineState);

	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	Graphics::SpriteVertex Vertices[4] = {};
	UpdateVertices(Vertices);
	static constexpr uint16_t Indices[6] =
	{
		0, 1, 2,
		1, 3, 2
	};
	Graphics::Renderer::GetInstance()->DrawSprite(
		Vertices, 4, Indices, 6);
}
/// <summary>
/// テクスチャ描画
/// </summary>
void Sprite::RenderTexture()
{
	Graphics::DirectX* DirextXInstance = Graphics::DirectX::GetInstance();
	ID3D12GraphicsCommandList* CommandList = DirextXInstance->GetCommandList();

	CommandList->SetGraphicsRootSignature(RootSignature);
	CommandList->SetPipelineState(PipelineState);
	Tex->Set(0);

	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	/*
	* DirectXは座標の中心が(0,0)で
	* 左上(-1.0,1.0)
	* 左下(-1.0,-1.0)
	* 右上(1.0,1.0)
	* 右下(1.0,-1.0)
	* 1920x1080 でも 1280x720の解像度でも変わらない
	*/
	Graphics::SpriteVertex Vertices[4] = {};
	UpdateVertices(Vertices);
	static constexpr uint16_t Indices[6] =
	{
		0, 1, 2,
		1, 3, 2
	};
	Graphics::Renderer::GetInstance()->DrawSprite(
		Vertices, 4, Indices, 6);
}

/// <summary>
/// 頂点座標変換
/// </summary>
void Sprite::UpdateVertices(Graphics::SpriteVertex* Vertices)
{
	
	/*
	* TODO:任意の座標にスプライトを描画できるようにする
	* テクスチャUVに関しては一旦固定値
	*/

	/*
	* 左上
	*/
	Vertices[0].Position = { 0.0f, 0.0f, 0.0f };
	Vertices[0].UV.x = 0.0f;
	Vertices[0].UV.y = 0.0f;

	/*
	* 左下
	*/
	Vertices[1].Position = { 0.0f, Size.y, 0.0f };
	Vertices[1].UV.x = 0.0f;
	Vertices[1].UV.y = 1.0f;

	/*
	* 右上
	*/
	Vertices[2].Position = { Size.x, 0.0f, 0.0f };
	Vertices[2].UV.x = 1.0f;
	Vertices[2].UV.y = 0.0f;

	/*
	* 右下
	*/
	Vertices[3].Position = { Size.x, Size.y, 0.0f };
	Vertices[3].UV.x = 1.0f;
	Vertices[3].UV.y = 1.0f;

	/*
	* 1. 原点(ローカル座標)の設定
	* 2. スケール(Scale)の設定
	* 3. 回転(Angle)の設定
	* 4. 移動(Position)の設定
	* 5. 頂点をDirectX用の座標系(X:-1 ~ 1, Y:-1 ~ 1)に変換
	*/

	/*
	* HINT 1
	* Pivotが(0.5f,0.5f)なら中心が軸
	* Sizeが使える xが幅、yが高さ
	*/

	/*
	* HINT 3
	* Angleは0~360の値だが sinf(),cosf()は0~2πの値
	* Math::PI,Math::RADが使える
	* 回転し座標を変換するには「Z軸回転行列」
	* 座標にZ軸回転行列をかけ算する
	*/
	const float theta = Angle * Math::RAD;
	const float sin = sinf(theta);
	const float cos = cosf(theta);
	
	/*
	* HINT 5
	* 画面内に頂点変換するには「平行投影行列(Z抜き)」
	* ただしアプリでは左上の座標を(0,0)とするが、DirectXでは左上が(-1,1)
	* 右下がアプリでは(Window::Width:1280, Window::Height:720)、DirectXでは(1,-1)
	*/
	for (int i = 0; i < 4; i++)
	{
		/*
		* 座標の中心の指定(ローカル座標)と拡縮
		*/
		Vertices[i].Position.x -= Pivot.x * Size.x;
		Vertices[i].Position.y -= Pivot.y * Size.y;
		Vertices[i].Position.x *= Scale.x;
		Vertices[i].Position.y *= Scale.y;

		/*
		* 座標の回転
		*/
		float x = Vertices[i].Position.x;
		float y = Vertices[i].Position.y;

		Vertices[i].Position.x = x * cos + y * sin;
		Vertices[i].Position.y = x * -sin + y * cos;

		/*
		* 座標の平行移動
		*/
		x = Vertices[i].Position.x + Position.x;
		y = Vertices[i].Position.y + Position.y;

		Vertices[i].Position.x = x * (2.0f / Window::Width) - 1.0f;
		Vertices[i].Position.y = y * (-2.0f / Window::Height) + 1.0f;
	}

}

/// <summary>
/// 座標の取得
/// </summary>
/// <returns>座標</returns>
const Math::Vector2& Sprite::GetPosition()const
{
	return Position;
}
/// <summary>
/// 座標の指定
/// </summary>
/// <param name="position">座標</param>
void Sprite::SetPosition(const Math::Vector2& position)
{
	Position = position;
}

/// <summary>
/// 軸の取得
/// </summary>
/// <returns>正規化された軸の座標</returns>
const Math::Vector2& Sprite::GetPivot()const
{
	return Pivot;
}
/// <summary>
/// 中心となる軸の指定
/// 座標は正規化された値を指定する
/// </summary>
/// <param name="pivot">軸 (X:0 ~ 1.0, Y:0 ~ 1.0f)</param>
void Sprite::SetPivot(const Math::Vector2& pivot)
{
	Pivot = pivot;
}

/// <summary>
/// 拡縮の取得
/// </summary>
/// <returns>拡縮の倍率</returns>
const Math::Vector2& Sprite::GetScale()const
{
	return Scale;
}
/// <summary>
/// 拡縮の倍率の指定
/// </summary>
/// <param name="scale">拡縮の倍率</param>
void Sprite::SetScale(const Math::Vector2& scale)
{
	Scale = scale;
}

/// <summary>
/// サイズの取得
/// </summary>
/// <returns>サイズ(幅, 高さ)</returns>
const Math::Vector2& Sprite::GetSize()const
{
	return Size;
}
/// <summary>
/// サイズの指定
/// </summary>
/// <param name="size">サイズ(幅, 高さ)</param>
void Sprite::SetSize(const Math::Vector2& size)
{
	Size = size;
}

/// <summary>
/// 回転の角度の取得
/// </summary>
/// <returns>回転の角度</returns>
float Sprite::GetAngle()const
{
	return Angle;
}
/// <summary>
/// 回転の角度の指定
/// </summary>
/// <param name="angle">回転の角度(0 ~ 360)</param>
void Sprite::SetAngle(float angle)
{
	Angle = angle;
}



/// <summary>
/// シェーダーの作成
/// </summary>
/// <returns>true:成功</returns>
bool Sprite::CreateShader()
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
				OutputDebugString((char*)ErrorBlob->GetBufferPointer());
				SafeRelease(ErrorBlob);
			}

			return ShaderBlob;
		};

	VertexShaderBlob = CompileShader("Assets/Shader/VertexShader.hlsl", "main", "vs_5_0");
	if (VertexShaderBlob == nullptr)
	{
		return false;
	}
	//	頂点シェーダーへの入力情報の設定
	InputLayout[0].SemanticName = "POSITION";
	InputLayout[0].SemanticIndex = 0;
	InputLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;//float3
	InputLayout[0].InputSlot = 0;
	InputLayout[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	InputLayout[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	InputLayout[0].InstanceDataStepRate = 0;

	PixelShaderBlob = CompileShader("Assets/Shader/PixelShader.hlsl", "main", "ps_5_0");
	if (PixelShaderBlob == nullptr)
	{
		return false;
	}
	return true;
}
bool Sprite::CreateTextureShader()
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
				OutputDebugString((char*)ErrorBlob->GetBufferPointer());
				SafeRelease(ErrorBlob);
			}

			return ShaderBlob;
		};

	VertexShaderBlob = CompileShader("Assets/Shader/VS_Texture.hlsl", "main", "vs_5_0");
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
	}
	InputLayout[0].SemanticName = "POSITION";
	InputLayout[0].SemanticIndex = 0;
	InputLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;//float3
	InputLayout[0].InputSlot = 0;

	InputLayout[1].SemanticName = "TEXCOORD";
	InputLayout[1].SemanticIndex = 0;
	InputLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;//float2
	InputLayout[1].InputSlot = 0;

	PixelShaderBlob = CompileShader("Assets/Shader/PS_Texture.hlsl", "main", "ps_5_0");
	if (PixelShaderBlob == nullptr)
	{
		return false;
	}
	return true;
}

/// <summary>
/// ルートシグネチャの作成
/// </summary>
/// <returns>true:成功</returns>
bool Sprite::CreateRootSignature()
{
	D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc = {};
	RootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//	ルートシグネチャ設定
	ID3DBlob* RootSignatureBlob = nullptr;
	ID3DBlob* ErrorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &RootSignatureBlob, &ErrorBlob);
	if (FAILED(hr))
	{
		OutputDebugString((char*)ErrorBlob->GetBufferPointer());
		SafeRelease(ErrorBlob);
		return false;
	}

	ID3D12Device* Device = Graphics::DirectX::GetInstance()->GetDevice();
	hr = Device->CreateRootSignature(0, RootSignatureBlob->GetBufferPointer(), RootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&RootSignature));
	SafeRelease(RootSignatureBlob);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}
bool Sprite::CreateTextureRootSignature()
{
	//	テクスチャパラメーター設定
	D3D12_DESCRIPTOR_RANGE Range = {};
	Range.BaseShaderRegister = 0;
	Range.NumDescriptors = 1;
	Range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	Range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//	ルートパラメーター設定
	D3D12_ROOT_PARAMETER RootParam = {};
	RootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	RootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	RootParam.DescriptorTable.pDescriptorRanges = &Range;
	RootParam.DescriptorTable.NumDescriptorRanges = 1;

	//	サンプラー設定
	D3D12_STATIC_SAMPLER_DESC SamplerDesc = {};
	SamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	SamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	SamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	SamplerDesc.MipLODBias = 0;
	SamplerDesc.MaxAnisotropy = 16;
	SamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_EQUAL;
	SamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
	SamplerDesc.MinLOD = 0;
	SamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	SamplerDesc.ShaderRegister = 0;
	SamplerDesc.RegisterSpace = 0;
	SamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc = {};
	RootSignatureDesc.NumParameters = 1;
	RootSignatureDesc.pParameters = &RootParam;
	RootSignatureDesc.NumStaticSamplers = 1;
	RootSignatureDesc.pStaticSamplers = &SamplerDesc;
	RootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//	ルートシグネチャ設定
	ID3DBlob* RootSignatureBlob = nullptr;
	ID3DBlob* ErrorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &RootSignatureBlob, &ErrorBlob);
	if (FAILED(hr))
	{
		OutputDebugString((char*)ErrorBlob->GetBufferPointer());
		SafeRelease(ErrorBlob);
		return false;
	}

	ID3D12Device* Device = Graphics::DirectX::GetInstance()->GetDevice();
	hr = Device->CreateRootSignature(0, RootSignatureBlob->GetBufferPointer(), RootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&RootSignature));
	SafeRelease(RootSignatureBlob);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}


/// <summary>
/// 描画パイプラインの作成
/// </summary>
/// <returns>true:成功</returns>
bool Sprite::CreatePipeline()
{
	PipelineStateDesc.pRootSignature = RootSignature;

	//コンパイルした頂点シェーダーの情報を格納
	PipelineStateDesc.VS.pShaderBytecode = VertexShaderBlob->GetBufferPointer();
	PipelineStateDesc.VS.BytecodeLength = VertexShaderBlob->GetBufferSize();
	PipelineStateDesc.InputLayout.NumElements = 1;
	PipelineStateDesc.InputLayout.pInputElementDescs = &InputLayout[0];

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
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

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
		return false;
	}
	return true;
}
bool Sprite::CreateTexturePipeline()
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
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

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
		return false;
	}
	return true;
}

/// <summary>
/// 深度ステンシルの初期設定の取得
/// </summary>
/// <returns></returns>
D3D12_DEPTH_STENCIL_DESC Sprite::GetDepthStencilDesc()
{
	D3D12_DEPTH_STENCIL_DESC Desc = {};
	Desc.DepthEnable = FALSE;
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

/// <summary>
/// ブレンドステートの初期設定の取得
/// </summary>
/// <returns></returns>
D3D12_BLEND_DESC Sprite::GetBlendStateDesc()
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

/// <summary>
/// ラスタライザーの初期設定の取得
/// </summary>
/// <returns></returns>
D3D12_RASTERIZER_DESC Sprite::GetRasterizerDesc()
{
	D3D12_RASTERIZER_DESC Desc = {};
	Desc.FillMode = D3D12_FILL_MODE_SOLID;
	Desc.CullMode = D3D12_CULL_MODE_NONE;
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