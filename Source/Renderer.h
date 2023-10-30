#pragma once
// Link library dependencies
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "winmm.lib")

#include "d3dx12.h"

#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#include "Core/Window.h"
#include "Renderer/Mesh.h"
#include "Renderer/IRenderer.h"
#include "Renderer/GraphicsContext.h"
#include "Renderer/DXUtils.h"
#include "Renderer/Camera.h"

DECLSPEC_ALIGN(256)
struct CBufferSceneData
{
    Mat4 wvp;
};

struct PerDrawConstants
{
    uint32 position_buffer_idx = 0;
    uint32 uv_buffer_idx = 0;
    uint32 scene_cbuffer_idx = 0;
};

class Renderer : public IRenderer
{
public:
    Renderer();
    Renderer(const Renderer&) = delete;                 // <-- No copy!
    Renderer& operator=(const Renderer&) = delete;      // <-/

    static Renderer* Get()
    {
        CHECK(gfx::renderer != nullptr);
        return (Renderer*)gfx::renderer;
    }

    virtual void Render() override final;
    virtual void Present() override final;

    void RecreateDepthBuffer(int32 width, int32 height);

private:
    using MeshData = CubeMeshData;

    Camera camera;

    // Per Frame Context
    std::array<ComPtr<ID3D12DescriptorHeap>, 2> descriptor_heaps_cbv_uav_srv;
    std::array<void*, 2> cbuffer_gpu_ptrs;

    CBufferSceneData cbuffer;
    std::array<ComPtr<ID3D12Resource>, 2> cbuffer_heaps;
    std::array<PerDrawConstants, 2> per_draw_constants_;

    ComPtr<ID3D12Resource> index_buffer_;
    D3D12_INDEX_BUFFER_VIEW index_buffer_view_;
    ComPtr<ID3D12Resource> vertex_pos_buffer_;
    ComPtr<ID3D12Resource> vertex_uv_buffer_;
    ComPtr<ID3D12Resource> depth_buffer_;

    ComPtr<ID3D12RootSignature> root_signature;
    ComPtr<ID3DBlob> serialized_root_signature;
    ComPtr<ID3D12PipelineState> pso;
};

IRenderer* CreateRenderer();
