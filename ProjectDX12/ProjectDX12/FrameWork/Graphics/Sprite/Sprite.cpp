#include "Sprite.h"
#include "../FrameWork/Graphics/DirectX/DirectX.h"
#include "../FrameWork/Graphics/Renderer/Renderer.h"
#include "../Graphics.h"
#include <d3dcompiler.h>
#include <cassert>
#include <d3d12.h>
#include"../FrameWork/System/Window/Window.h"
#include "math.h"
#include <Math/Math.h>

#pragma comment(lib,"d3dcompiler.lib")

Sprite::Sprite() {}

Sprite::~Sprite()
{
    SafeRelease(VSBlob);
    SafeRelease(PSBlob);
    SafeRelease(RootSignature);
    SafeRelease(PipelineState);

    Tex = nullptr; // 非所有なので delete しない
}

bool Sprite::Create()
{
    if (!CreateShader()) return false;
    if (!CreateRootSignature()) return false;
    if (!CreatePipeline()) return false;

    assert(PipelineState && RootSignature);
    return true;
}

void Sprite::SetTexture(Texture* texture)
{
    Tex = texture;
    if (Tex)
    {
        Size.x = Tex->GetWidth();
        Size.y = Tex->GetHeight();
    }
}

void Sprite::Draw()
{
    // 防御コード
    if (!Tex || !PipelineState || !RootSignature)
        return;

    auto* dx = Graphics::DirectX::GetInstance();
    auto* cmd = dx->GetCommandList();

    cmd->SetGraphicsRootSignature(RootSignature);
    cmd->SetPipelineState(PipelineState);

    Tex->Set(0);

    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    Graphics::SpriteVertex vertices[4]{};
    UpdateVertices(vertices);

    static constexpr uint16_t indices[6] =
    {
        0,1,2,
        1,3,2
    };

    Graphics::Renderer::GetInstance()->DrawSprite(
        vertices, 4, indices, 6
    );
}

void Sprite::UpdateVertices(Graphics::SpriteVertex* v)
{
    const float w = Size.x * Scale.x;
    const float h = Size.y * Scale.y;

    const float left = -Pivot.x * w;
    const float right = left + w;
    const float top = -Pivot.y * h;
    const float bottom = top + h;

    v[0] = { { left,  top, 0},    {0,0} };
    v[1] = { { right, top, 0},    {1,0} };
    v[2] = { { left,  bottom, 0}, {0,1} };
    v[3] = { { right, bottom, 0}, {1,1} };

    const float theta = Angle * Math::RAD;
    const float sin = sinf(theta);
    const float cos = cosf(theta);

    for (int i = 0; i < 4; ++i)
    {
        v[i].Position.x += Position.x;
        v[i].Position.y += Position.y;

        /*
        * 座標の回転
        */
        float x = v[i].Position.x;
        float y = v[i].Position.y;

        v[i].Position.x = x * cos + y * sin;
        v[i].Position.y = x * -sin + y * cos;

        /*
        * 座標の平行移動
        */
        x = v[i].Position.x + Position.x;
        y = v[i].Position.y + Position.y;

        v[i].Position.x = x * (2.0f / Window::Width) - 1.0f;
        v[i].Position.y = y * (-2.0f / Window::Height) + 1.0f;
    }
}

/* ===== DX12 初期化 ===== */

bool Sprite::CreateShader()
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

    VSBlob = Compile(L"Assets/Shader/VS_Texture.hlsl", "main", "vs_5_0");
    PSBlob = Compile(L"Assets/Shader/PS_Texture.hlsl", "main", "ps_5_0");

    return VSBlob && PSBlob;
}

bool Sprite::CreateRootSignature()
{
    D3D12_DESCRIPTOR_RANGE range{};
    range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    range.NumDescriptors = 1;
    range.BaseShaderRegister = 0;

    D3D12_ROOT_PARAMETER param{};
    param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    param.DescriptorTable.NumDescriptorRanges = 1;
    param.DescriptorTable.pDescriptorRanges = &range;
    param.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    D3D12_STATIC_SAMPLER_DESC sampler{};
    sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    sampler.AddressU = sampler.AddressV = sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.ShaderRegister = 0;
    sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    D3D12_ROOT_SIGNATURE_DESC desc{};
    desc.NumParameters = 1;
    desc.pParameters = &param;
    desc.NumStaticSamplers = 1;
    desc.pStaticSamplers = &sampler;
    desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ID3DBlob* blob = nullptr;
    ID3DBlob* err = nullptr;

    HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &err);
    if (FAILED(hr)) return false;

    hr = Graphics::DirectX::GetInstance()->GetDevice()->CreateRootSignature(
        0, blob->GetBufferPointer(), blob->GetBufferSize(),
        IID_PPV_ARGS(&RootSignature));

    SafeRelease(blob);
    return SUCCEEDED(hr);
}

bool Sprite::CreatePipeline()
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};
    desc.pRootSignature = RootSignature;

    desc.VS = { VSBlob->GetBufferPointer(), VSBlob->GetBufferSize() };
    desc.PS = { PSBlob->GetBufferPointer(), PSBlob->GetBufferSize() };

    D3D12_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,
          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
        { "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,
          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
    };

    desc.InputLayout.pInputElementDescs = layout;
    desc.InputLayout.NumElements = _countof(layout);

    desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    desc.NumRenderTargets = 1;
    desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleMask = UINT_MAX;

    /* ===== ラスタライザ設定 ===== */
    D3D12_RASTERIZER_DESC raster{};
    raster.FillMode = D3D12_FILL_MODE_SOLID;
    raster.CullMode = D3D12_CULL_MODE_NONE;
    raster.FrontCounterClockwise = FALSE;
    raster.DepthClipEnable = FALSE;
    desc.RasterizerState = raster;

    /* ===== ブレンド設定（α未使用） ===== */
    D3D12_BLEND_DESC blend{};
    blend.RenderTarget[0].BlendEnable = FALSE;
    blend.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    desc.BlendState = blend;

    /* ===== 深度無効（2D） ===== */
    D3D12_DEPTH_STENCIL_DESC depth{};
    depth.DepthEnable = FALSE;
    depth.StencilEnable = FALSE;
    desc.DepthStencilState = depth;

    HRESULT hr =
        Graphics::DirectX::GetInstance()->GetDevice()
        ->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&PipelineState));

    return SUCCEEDED(hr);
}

/* ===== Transform Setter / Getter ===== */

void Sprite::SetPosition(const Math::Vector2& pos)
{
    Position = pos;
}

void Sprite::SetPivot(const Math::Vector2& pivot)
{
    Pivot = pivot;
}

void Sprite::SetScale(const Math::Vector2& scale)
{
    Scale = scale;
}

void Sprite::SetSize(const Math::Vector2& size)
{
    Size = size;
}

void Sprite::SetAngle(float angle)
{
    Angle = angle;
}

const Math::Vector2& Sprite::GetPosition() const
{
    return Position;
}

const Math::Vector2& Sprite::GetPivot() const
{
    return Pivot;
}

const Math::Vector2& Sprite::GetScale() const
{
    return Scale;
}

const Math::Vector2& Sprite::GetSize() const
{
    return Size;
}

float Sprite::GetAngle() const
{
    return Angle;
}
