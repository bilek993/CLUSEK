struct HS_INPUT
{
    float3 Position : POSITION;
    float2 TextureCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

struct HS_OUTPUT
{
    float3 Position : POSITION;
    float2 TextureCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

struct PatchTess
{
    float EdgeTess[4] : SV_TessFactor;
    float InsideTess[2] : SV_InsideTessFactor;
};

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
    output.Normal = patches[i].Normal;
    output.Tangent = patches[i].Tangent;

    return output;
}

PatchTess ConstantHS(InputPatch<HS_INPUT, 4> patch, uint patchID : SV_PrimitiveID) // TODO: Add logic in this constant function
{
    PatchTess constantOutput;
    
    constantOutput.EdgeTess[0] = 0.0f;
    constantOutput.EdgeTess[1] = 0.0f;
    constantOutput.EdgeTess[2] = 0.0f;
    constantOutput.EdgeTess[3] = 0.0f;
    
    constantOutput.InsideTess[0] = 0.0f;
    constantOutput.InsideTess[1] = 0.0f;
    
    return constantOutput;
}