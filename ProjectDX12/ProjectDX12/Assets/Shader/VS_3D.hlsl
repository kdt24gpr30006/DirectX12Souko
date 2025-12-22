#include "Header3D.hlsli"

VStoPS main(VertexInput input)
{
    VStoPS output = (VStoPS)0;
    
    //姿勢 * ビュー行列 * プロジェクション行列
    float4 Position = float4(input.Position, 1.0f);
    Position = mul(Position, Transform);
    Position = mul(Position, View);
    Position = mul(Position, Projection);
    output.Position = Position;
    
    //法線変換
    float4 Normal = float4(input.Normal, 0);
    Normal = mul(Normal, Transform);
    output.Normal = normalize(Normal.xyz);
    
    //テクスチャUV
    output.UV = input.UV;
    //モデル全体の色(マテリアルカラー)
    output.Color = Color;
    
    return output;
}