cbuffer ModelBuffer : register(b0)
{
    matrix Transform;
    matrix View;
    matrix Projection;
    float4 Color;
};

struct VSInput
{
    float3 Position : POSITION;
    float2 UV : TEXCOORD;
    float4 VertColor : COLOR;
};

struct VSOutput
{
    float4 Position : SV_Position;
    float2 UV : TEXCOORD;
    float4 Color : COLOR;
};

VSOutput main(VSInput input)
{
    VSOutput o;
    float4 pos = mul(float4(input.Position, 1), Transform);
    pos = mul(pos, View);
    pos = mul(pos, Projection);
    o.Position = pos;
    o.UV = input.UV;
    o.Color = Color;
    return o;
}
