struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TextureCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    return float4(1.0f, 1.0f, 1.0f, 0.5f); // TODO: Implement main function
}