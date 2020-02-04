static const float MIN_TESSELATION = 0;
static const float MAX_TESSELATION = 6;

static const float MIN_DISTANCE = 10.0F;
static const float MAX_DISTANCE = 3000.0F;

cbuffer TerrainBuffer : register(b0)
{
    float4 FrustumPlanes[6];
}

cbuffer CameraBuffer : register(b1)
{
    float3 CameraPosition;
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

bool IsNotVisibileByCamera(float3 center, float3 extents)
{
    for (int i = 0; i < 6; i++)
    {
        if (AabbBehindPlaneTest(center, extents, FrustumPlanes[i]))
        {
            return true;
        }
    }
    
    return false;
}

float CalculateTesselationFactor(float3 patchPosition)
{
    float distanceFromCamera = distance(patchPosition, CameraPosition);
    float shift = saturate((distanceFromCamera - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE));
    
    return pow(2, (lerp(MAX_TESSELATION, MIN_TESSELATION, shift)));
}

PatchTess ConstantHS(InputPatch<HS_INPUT, 4> patch, uint patchID : SV_PrimitiveID)
{
    PatchTess constantOutput;
    
    float minY = patch[0].BoundsY.x;
    float maxY = patch[0].BoundsY.y;
    
    float3 vMin = float3(patch[0].Position.x, minY, patch[0].Position.z);
    float3 vMax = float3(patch[3].Position.x, maxY, patch[3].Position.z);
    
    float3 boxCenter = 0.5f * (vMin + vMax);
    float3 boxExtents = 0.5f * (vMax - vMin);
    
    if (IsNotVisibileByCamera(boxCenter, boxExtents))
    {
        constantOutput.EdgeTess[0] = 0.0f;
        constantOutput.EdgeTess[1] = 0.0f;
        constantOutput.EdgeTess[2] = 0.0f;
        constantOutput.EdgeTess[3] = 0.0f;
    
        constantOutput.InsideTess[0] = 0.0f;
        constantOutput.InsideTess[1] = 0.0f;
        
        return constantOutput;
    }
    else
    {
        float3 edges[4];
        edges[0] = 0.5f * (patch[0].Position + patch[2].Position);
        edges[1] = 0.5f * (patch[0].Position + patch[1].Position);
        edges[2] = 0.5f * (patch[1].Position + patch[3].Position);
        edges[3] = 0.5f * (patch[2].Position + patch[3].Position);
        
        float3 center = 0.25f * (patch[0].Position + patch[1].Position + patch[2].Position + patch[3].Position);
        
        constantOutput.EdgeTess[0] = CalculateTesselationFactor(edges[0]);
        constantOutput.EdgeTess[1] = CalculateTesselationFactor(edges[1]);
        constantOutput.EdgeTess[2] = CalculateTesselationFactor(edges[2]);
        constantOutput.EdgeTess[3] = CalculateTesselationFactor(edges[3]);
    
        constantOutput.InsideTess[0] = CalculateTesselationFactor(center);
        constantOutput.InsideTess[1] = constantOutput.InsideTess[0];
        
        return constantOutput;
    }
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