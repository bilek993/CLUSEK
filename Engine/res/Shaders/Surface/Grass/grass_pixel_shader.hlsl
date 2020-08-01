struct PS_INPUT
{
    float4 Position : SV_POSITION;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	// TODO: Add implementation code
    return float4(1.0, 0.0, 0.0f, 1.0f);
}