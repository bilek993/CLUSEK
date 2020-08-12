static const float LOD_GRID_SCALE = 10.0f;

void PerformLodTransition(float4 position, float percentageCoverage, bool invertedCoverage, float gridScale)
{
    if (invertedCoverage)
    {
        if (!(fmod(position.x, gridScale) > min(percentageCoverage * gridScale, gridScale) || fmod(position.y, gridScale) > min(percentageCoverage * gridScale, gridScale)))
            discard;
    }
    else
    {
        if (fmod(position.x, gridScale) > min(percentageCoverage * gridScale, gridScale) || fmod(position.y, gridScale) > min(percentageCoverage * gridScale, gridScale))
            discard;
    }
}