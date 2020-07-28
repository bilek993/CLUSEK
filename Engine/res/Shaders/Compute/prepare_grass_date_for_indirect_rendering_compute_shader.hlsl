RWStructuredBuffer<uint> CounterFromAppendBuffer : register(u0);
RWStructuredBuffer<uint> IndexInstanceIndirectArgs : register(u1);

[numthreads(1, 1, 1)]
void main()
{
    IndexInstanceIndirectArgs[0] = 6; // TODO: Change this (IndexCountPerInstance)
    IndexInstanceIndirectArgs[1] = CounterFromAppendBuffer[0];
    IndexInstanceIndirectArgs[2] = 0;
    IndexInstanceIndirectArgs[3] = 0;
    IndexInstanceIndirectArgs[4] = 0;
}