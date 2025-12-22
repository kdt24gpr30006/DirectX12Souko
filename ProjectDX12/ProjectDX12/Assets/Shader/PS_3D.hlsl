#include "Header3D.hlsli"

//	メインテクスチャ
Texture2D MainTexture : register(t0);
SamplerState Sampler : register(s0);

float ToonShadow(float DotValue)
{
    if (DotValue >= 0.5f)
    {
        return 1.0f;
    }
    else
    {
        return 0.5f;
    }
}

//float4 main(VStoPS input) : SV_Target0
//{
//    float3 normal = input.Normal;
//    normal *= 0.5f;
//    normal += 0.5f;
//    float4 OutputColor = float4(normal, 1.0f);
//    return OutputColor;
//}

float4 main(VStoPS input) : SV_Target0
{
    float4 OutputColor = MainTexture.Sample(Sampler, input.UV);
    //float4 OutputColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    OutputColor *= input.Color;
    //float4 OutputColor = input.Color;
    
    float3 LightVector = normalize(float3(-1.0f, -1.0f, -1.0f));
    //内積の計算の為にライト方向を逆にする
    LightVector = -LightVector;
    
    //  法線と光の内積をとる
    float d = dot(LightVector, input.Normal);
    //  -1 ~ 1 の範囲から 0 ~ 1へ変換する
    d *= 0.5f;
    d += 0.5f;
    float shadow = saturate(d + 0.2f);
    //float shadow = ToonShadow(d);
    OutputColor.rgb *= shadow;
    return OutputColor;
}