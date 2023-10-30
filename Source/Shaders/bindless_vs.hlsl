#include "common.hlsl"

cbuffer PerDrawConstants : register(b0, space0)
{
    uint32 pos_buffer_index;
    uint32 uv_buffer_index;
    uint32 scene_data_buffer_index;
};

struct SceneData 
{
    float4x4 wvp;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : UV0;
};

VSOutput Main(uint vertex_id : SV_VertexID)
{
    StructuredBuffer<float4> pos_buffer = ResourceDescriptorHeap[pos_buffer_index];
    StructuredBuffer<float2> uv_buffer = ResourceDescriptorHeap[uv_buffer_index];
    ConstantBuffer<SceneData> scene_data = ResourceDescriptorHeap[scene_data_buffer_index];
    float4 pos = pos_buffer[vertex_id];
    float2 uv = uv_buffer[vertex_id];

    VSOutput output;
    output.pos = mul(pos, scene_data.wvp);
    output.uv = uv;
    return output;
}
