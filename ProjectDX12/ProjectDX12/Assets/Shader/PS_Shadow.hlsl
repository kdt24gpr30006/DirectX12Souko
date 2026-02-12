Texture2D MainTexture : register(t0);
SamplerState Sampler : register(s0);

struct PSInput
{
    float4 Position : SV_Position;
    float2 UV : TEXCOORD;
    float4 Color : COLOR;
};

float4 main(PSInput input) : SV_Target0
{
    float4 texColor = MainTexture.Sample(Sampler, input.UV);
    float alpha = texColor.a * input.Color.a;
    if (alpha <= 0)
    {
        discard;
    }
    return float4(input.Color.rgb, alpha);
}
