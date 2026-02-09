#include "Header.hlsli"

//	メインテクスチャ
Texture2D MainTexture : register(t0);
SamplerState Sampler : register(s0);

float4 main(VStoPS input) : SV_Target0
{
    float4 texColor = MainTexture.Sample(Sampler, input.UV);
    if (texColor.a <= 0)
    {
        discard;
    }
    float3 rgb = texColor.rgb * input.Color.rgb;
    return float4(rgb, input.Color.a);
}
