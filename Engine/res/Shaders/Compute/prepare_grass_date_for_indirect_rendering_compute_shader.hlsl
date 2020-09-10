RWBuffer<uint> CounterFromAppendBuffer : register(u0);
RWBuffer<uint> IndexInstanceIndirectArgs : register(u1);

cbuffer GrassIndirectModelInfoBuffer : register(b0)
{
    uint IndexCountPerInstance;
    uint MaxNumberOfInstances;
};

[numthreads(1, 1, 1)]
void main()
{
    IndexInstanceIndirectArgs[0] = IndexCountPerInstance; // IndexCountPerInstance (the number of indices read from the index buffer for each instance)
    IndexInstanceIndirectArgs[1] = min(CounterFromAppendBuffer[0], MaxNumberOfInstances); // InstanceCount (the number of instances to draw)
    IndexInstanceIndirectArgs[2] = 0; // StartIndexLocation (the location of the first index read by the GPU from the index buffer)
    IndexInstanceIndirectArgs[3] = 0; // BaseVertexLocation (a value added to each index before reading a vertex from the vertex buffer)
    IndexInstanceIndirectArgs[4] = 0; // StartInstanceLocation (a value added to each index before reading per-instance data from a vertex buffer)
}