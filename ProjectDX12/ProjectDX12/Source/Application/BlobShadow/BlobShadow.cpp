#include "BlobShadow.h"
#include "Graphics/DirectX/DirectX.h"
#include "Graphics/Renderer/Renderer.h"
#include "Graphics/Graphics.h"
#include "Graphics/ConstantBuffer/ConstantBuffer.h"
#include "Graphics/Texture/Texture.h"
#include "System/Camera/Camera.h"
#include "Math/Vector3/Vector3.h"
#include <Math/Quaternion/Quaternion.h>
#include <d3dcompiler.h>
#include <array>

#pragma comment(lib, "d3dcompiler.lib")

BlobShadow::BlobShadow() {}

BlobShadow::~BlobShadow()
{
    Release();
}

bool BlobShadow::Create()
{
    if (!CreateShader()) return false;
    if (!CreateRootSignature()) return false;
    if (!CreatePipeline()) return false;

    BufferInfo.Transform = Math::Matrix::identity;
    BufferInfo.View = Math::Matrix::identity;
    BufferInfo.Projection = Math::Matrix::identity;
    BufferInfo.ShadowColor = ShadowColor;

    TransformBuffer = new ConstantBuffer();
    if (!TransformBuffer->Create(sizeof(ShadowConstantBuffer)))
    {
        return false;
    }
    TransformBuffer->Update(&BufferInfo);

    return true;
}

void BlobShadow::SetTexture(Texture* texture)
{
    Tex = texture;
}

void BlobShadow::Draw(const Math::Vector3& worldPos, float scale)
{
    if (!Camera::Main || !Tex || !PipelineState || !RootSignature)
        return;

    // Transform: XZ平面にscale倍、Y方向に少し浮かせてZファイティング回避
    Math::Vector3 shadowPos = { worldPos.x, worldPos.y + 0.05f, worldPos.z };
    Math::Vector3 shadowScale = { scale, 1.0f, scale };

    BufferInfo.Transform.Update(shadowPos, Math::Quaternion::Identity, shadowScale);
    BufferInfo.Transform = Math::Matrix::Transpose(BufferInfo.Transform);
    BufferInfo.View = Math::Matrix::Transpose(Camera::Main->GetView());
    BufferInfo.Projection = Math::Matrix::Transpose(Camera::Main->GetProjection());
    BufferInfo.ShadowColor = ShadowColor;
    TransformBuffer->Update(&BufferInfo);

    auto* dx = Graphics::DirectX::GetInstance();
    auto* cmd = dx->GetCommandList();

    cmd->SetGraphicsRootSignature(RootSignature);
    cmd->SetPipelineState(PipelineState);
    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    TransformBuffer->Set(0);
    Tex->Set(1);

    // XZ平面上の単位クアッド（Transform で拡大・移動される）
    Graphics::SpriteVertex vertices[4] =
    {
        { {-0.5f, 0.0f, -0.5f}, {0.0f, 0.0f}, Color::White },
        { { 0.5f, 0.0f, -0.5f}, {1.0f, 0.0f}, Color::White },
        { {-0.5f, 0.0f,  0.5f}, {0.0f, 1.0f}, Color::White },
        { { 0.5f, 0.0f,  0.5f}, {1.0f, 1.0f}, Color::White },
    };

    static constexpr uint16_t indices[6] = { 0, 1, 2, 1, 3, 2 };

    Graphics::Renderer::GetInstance()->DrawSprite(vertices, 4, indices, 6);
}

void BlobShadow::Release()
{
    // NOTE: D3D12 COM オブジェクトはデバイス破棄時に自動回収されるため
    //       終了時のアクセス違反を回避するためポインタのみクリアする
    PipelineState = nullptr;
    RootSignature = nullptr;
    VSBlob = nullptr;
    PSBlob = nullptr;

    if (TransformBuffer)
    {
        delete TransformBuffer;
        TransformBuffer = nullptr;
    }

    Tex = nullptr;
}

bool BlobShadow::CreateShader()
{
    auto Compile = [](const wchar_t* path, const char* entry, const char* model)
        {
            ID3DBlob* blob = nullptr;
            ID3DBlob* err = nullptr;

            HRESULT hr = D3DCompileFromFile(
                path, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                entry, model,
                D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
                0, &blob, &err);

            if (FAILED(hr))
            {
                if (err) OutputDebugStringA((char*)err->GetBufferPointer());
                SafeRelease(err);
                return (ID3DBlob*)nullptr;
            }
            return blob;
        };

    VSBlob = Compile(L"Assets/Shader/VS_Shadow.hlsl", "main", "vs_5_0");
    PSBlob = Compile(L"Assets/Shader/PS_Shadow.hlsl", "main", "ps_5_0");

    return VSBlob && PSBlob;
}

bool BlobShadow::CreateRootSignature()
{
    // FbxMeshと同じレイアウト: param0 = CBV(b0), param1 = SRV(t0)
    std::array<D3D12_DESCRIPTOR_RANGE, 2> range{};

    range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    range[0].NumDescriptors = 1;
    range[0].BaseShaderRegister = 0;
    range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    range[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    range[1].NumDescriptors = 1;
    range[1].BaseShaderRegister = 0;
    range[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    std::array<D3D12_ROOT_PARAMETER, 2> param{};

    param[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    param[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    param[0].DescriptorTable.pDescriptorRanges = &range[0];
    param[0].DescriptorTable.NumDescriptorRanges = 1;

    param[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    param[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    param[1].DescriptorTable.pDescriptorRanges = &range[1];
    param[1].DescriptorTable.NumDescriptorRanges = 1;

    D3D12_STATIC_SAMPLER_DESC sampler{};
    sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    sampler.ShaderRegister = 0;
    sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    D3D12_ROOT_SIGNATURE_DESC desc{};
    desc.NumParameters = static_cast<UINT>(param.size());
    desc.pParameters = param.data();
    desc.NumStaticSamplers = 1;
    desc.pStaticSamplers = &sampler;
    desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ID3DBlob* blob = nullptr;
    ID3DBlob* err = nullptr;

    HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &err);
    if (FAILED(hr))
    {
        if (err) OutputDebugStringA((char*)err->GetBufferPointer());
        SafeRelease(err);
        return false;
    }

    hr = Graphics::DirectX::GetInstance()->GetDevice()->CreateRootSignature(
        0, blob->GetBufferPointer(), blob->GetBufferSize(),
        IID_PPV_ARGS(&RootSignature));

    SafeRelease(blob);
    return SUCCEEDED(hr);
}

bool BlobShadow::CreatePipeline()
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};
    desc.pRootSignature = RootSignature;

    desc.VS = { VSBlob->GetBufferPointer(), VSBlob->GetBufferSize() };
    desc.PS = { PSBlob->GetBufferPointer(), PSBlob->GetBufferSize() };

    // SpriteVertex と同じレイアウト: Position(float3), UV(float2), Color(float4)
    D3D12_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    desc.InputLayout.pInputElementDescs = layout;
    desc.InputLayout.NumElements = _countof(layout);

    desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    desc.NumRenderTargets = 1;
    desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    desc.SampleDesc.Count = 1;
    desc.SampleMask = UINT_MAX;

    /* ===== ラスタライザ: カリングなし ===== */
    D3D12_RASTERIZER_DESC raster{};
    raster.FillMode = D3D12_FILL_MODE_SOLID;
    raster.CullMode = D3D12_CULL_MODE_NONE;
    raster.FrontCounterClockwise = FALSE;
    raster.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    raster.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    raster.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    raster.DepthClipEnable = TRUE;
    desc.RasterizerState = raster;

    /* ===== アルファブレンド有効 ===== */
    D3D12_BLEND_DESC blend{};
    blend.RenderTarget[0].BlendEnable = TRUE;
    blend.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    blend.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    blend.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    blend.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    blend.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    blend.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    blend.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    desc.BlendState = blend;

    /* ===== 深度テスト有効、深度書き込み無効 ===== */
    D3D12_DEPTH_STENCIL_DESC depth{};
    depth.DepthEnable = TRUE;
    depth.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    depth.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    depth.StencilEnable = FALSE;
    desc.DepthStencilState = depth;

    HRESULT hr =
        Graphics::DirectX::GetInstance()->GetDevice()
        ->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&PipelineState));

    return SUCCEEDED(hr);
}
