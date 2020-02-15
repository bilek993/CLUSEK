float3 GetSamplingVector(uint3 ThreadID, float outputWidth, float outputHeight, float outputDepth)
{
    float2 st = ThreadID.xy / float2(outputWidth, outputHeight);
    float2 uv = 2.0 * float2(st.x, 1.0 - st.y) - 1.0;

    switch (ThreadID.z)
    {
        case 0:
            return normalize(float3(1.0, uv.y, -uv.x));
        case 1:
            return normalize(float3(-1.0, uv.y, uv.x));
        case 2:
            return normalize(float3(uv.x, 1.0, -uv.y));
        case 3:
            return normalize(float3(uv.x, -1.0, uv.y));
        case 4:
            return normalize(float3(uv.x, uv.y, 1.0));
        case 5:
            return normalize(float3(-uv.x, uv.y, -1.0));
        default:
            return float3(0.0f, 0.0f, 0.0f);
    }
}