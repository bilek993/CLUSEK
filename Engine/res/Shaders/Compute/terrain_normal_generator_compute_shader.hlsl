RWTexture2D<float4> OutputNormalTexture : register(u0);

[numthreads(32, 32, 1)]
void main(uint2 threadID : SV_DispatchThreadID)
{
    OutputNormalTexture[threadID] = float4(0.0f, 1.0f, 0.0f, 1.0f);
}