RWBuffer<uint> CounterFromAppendBuffer : register(u0);
RWBuffer<uint> IndexInstanceIndirectArgs : register(u1);

cbuffer GrassIndirectModelInfoBuffer : register(b0)
{
    uint IndexCountPerInstance;
};

[numthreads(1, 1, 1)]
void main()
{
    IndexInstanceIndirectArgs[0] = IndexCountPerInstance;
    IndexInstanceIndirectArgs[1] = CounterFromAppendBuffer[0];
    IndexInstanceIndirectArgs[2] = 0;
    IndexInstanceIndirectArgs[3] = 0;
    IndexInstanceIndirectArgs[4] = 0;
}