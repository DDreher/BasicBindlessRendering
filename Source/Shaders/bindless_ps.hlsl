#include "common.hlsl"

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv : UV0;
};

float4 Main(PSInput input) : SV_TARGET
{
    //float4 color = tex.Sample(tex_sampler, input.uv);
    float4 color = float4(input.uv.x, input.uv.y, 0.0f, 1.0f);
    return color;
}
