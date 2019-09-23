RWTexture2DArray<float4> OutputTexture : register(u0);

[numthreads(32, 32, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    OutputTexture[threadID] = float4(1.0f, 0.0f, 0.0f, 1.0f);
}