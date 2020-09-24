cbuffer ShadowBuffer : register(b0)
{
    float4x4 WorldLightMatrix;
}

cbuffer WorldMatrixBuffer : register(b1)
{
    float4x4 WorldMatrix;
}

cbuffer TimeBuffer : register(b2)
{
    float Time;
}

cbuffer WindBuffer : register(b3)
{
    float HightWindSpeed;
    float HightWindScale;
    float HightWindBase;
    float LocalWindSpeed;
    float LocalWindScale;
    bool HightWindEnabled;
    bool LocalWindEnabled;
}

struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TextureCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 TextureCoord : TEXCOORD;
};

float3 ApplyHightWind(float3 vertexPosition, float3 worldPosition)
{
    float timeScaled = Time * HightWindSpeed;
	
    float3 padding;
    padding.x = (2.0f * sin(1.0f * (worldPosition.x + worldPosition.y + worldPosition.z + timeScaled))) + 1.0f;
    padding.y = 0.0f;
    padding.z = (1.0f * sin(2.0f * (worldPosition.x + worldPosition.y + worldPosition.z + timeScaled))) + 0.5f;

    float bendingStrength = (vertexPosition.y * HightWindScale) + HightWindBase;
    bendingStrength *= bendingStrength;
    bendingStrength = (bendingStrength * bendingStrength) - bendingStrength;

    padding *= bendingStrength;
    
    return vertexPosition + padding;
}

VS_OUTPUT main(VS_INPUT input)
{
    float3 position = input.Position;
    if (HightWindEnabled)
        position = ApplyHightWind(position, mul(float4(0.0f, 0.0f, 0.0f, 1.0f), WorldMatrix).xyz);
	
    VS_OUTPUT output;
    output.Position = mul(float4(position, 1.0f), WorldLightMatrix);
    output.TextureCoord = input.TextureCoord;

    return output;
}