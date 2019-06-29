struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float3 Color : COLOR;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    return float4(input.Color, 1.0f);
}