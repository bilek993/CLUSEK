cbuffer TerrainBuffer : register(b0)
{
    float4 FrustumPlanes[6];
}

struct HS_INPUT
{
    float3 Position : POSITION;
    float2 TextureCoord : TEXCOORD;
    float2 BoundsY : BOUNDS_Y;
};

struct HS_OUTPUT
{
    float3 Position : POSITION;
    float2 TextureCoord : TEXCOORD;
};

struct PatchTess
{
    float EdgeTess[4] : SV_TessFactor;
    float InsideTess[2] : SV_InsideTessFactor;
};

bool AabbBehindPlaneTest(float3 center, float3 extents, float4 plane)
{
    float3 n = abs(plane.xyz);
    
    float r = dot(extents, n);
    float s = dot(float4(center, 1.0f), plane);
    
    return (s + r) < 0.0f;
}

bool IsVisibileByCamera(float3 center, float3 extents)
{
    for (int i = 0; i < 6; i++)
    {
        if (AabbBehindPlaneTest(center, extents, FrustumPlanes[i]))
        {
            return false;
        }
    }
    
    return true;
}

PatchTess ConstantHS(InputPatch<HS_INPUT, 4> patch, uint patchID : SV_PrimitiveID)
{
    PatchTess constantOutput;
    
    float minY = patch[0].BoundsY.x;
    float maxY = patch[0].BoundsY.y;
    
    float3 vMin = float3(patch[2].Position.x, minY, patch[2].Position.z);
    float3 vMax = float3(patch[1].Position.x, maxY, patch[1].Position.z);
    
    float3 boxCenter = 0.5f * (vMin + vMax);
    float3 boxExtents = 0.5f * (vMax - vMin);
    
    if (IsVisibileByCamera(boxCenter, boxExtents))
    {
        constantOutput.EdgeTess[0] = 1.0f;
        constantOutput.EdgeTess[1] = 1.0f;
        constantOutput.EdgeTess[2] = 1.0f;
        constantOutput.EdgeTess[3] = 1.0f;
    
        constantOutput.InsideTess[0] = 1.0f;
        constantOutput.InsideTess[1] = 1.0f;
    }
    else
    {
        constantOutput.EdgeTess[0] = 0.0f;
        constantOutput.EdgeTess[1] = 0.0f;
        constantOutput.EdgeTess[2] = 0.0f;
        constantOutput.EdgeTess[3] = 0.0f;
    
        constantOutput.InsideTess[0] = 0.0f;
        constantOutput.InsideTess[1] = 0.0f;
    }
    
    return constantOutput;
}

[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]
HS_OUTPUT main(InputPatch<HS_INPUT, 4> patches, uint i : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    HS_OUTPUT output;
    output.Position = patches[i].Position;
    output.TextureCoord = patches[i].TextureCoord;

    return output;
}