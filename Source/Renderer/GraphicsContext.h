#pragma once
#include <dxgi1_6.h>
#include "d3dx12.h"

#include "Renderer/DXUtils.h"
#include "Renderer/Camera.h"

class IRenderer;
class Window;

namespace gfx
{
    void Init(Window* window);
    void Shutdown();
    bool IsInitialized();

    void SetViewport(float width, float height);
    const D3D12_VIEWPORT& GetViewport();
    void SetRenderResolution(uint32 width, uint32 height);
    Vec2 GetRenderResolution();
    void CreateSwapchain(uint32 width, uint32 height, uint32 num_buffers, const HWND& hwnd);

    ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE type);

    ComPtr<ID3D12GraphicsCommandList> CreateGraphicsCommandList(const ComPtr<ID3D12CommandAllocator>& allocator, D3D12_COMMAND_LIST_TYPE type);

    ComPtr<ID3D12Fence> CreateFence();
    void Signal(const ComPtr<ID3D12CommandQueue>& queue, const ComPtr<ID3D12Fence>& fence, uint64 signal_value);
    
    /**
     * @brief Create OS event handle to block CPU thread until a fence has been signaled.
     */
    HANDLE CreateEventHandle();

    /**
     * @brief Stalls thread until the fence reaches the given value
     * @param fence The fence to signal
     * @param value The fence value to wait for
     * @param fence_event The OS event fired when the fence reaches the given value. Used to stall the thread.
     */
    void WaitForFence(const ComPtr<ID3D12Fence>& fence, uint64 value, HANDLE fence_event);

    /**
     * @brief Stalls thread until given command queue is empty
     * @param queue The command queue to flush
     * @param fence The fence to signal
     * @param fence_value The fence value to wait for
     * @param fence_event The OS event fired when the fence reaches the given value. Used to stall the thread.
     */
    void FlushQueue(const ComPtr<ID3D12CommandQueue>& queue, const ComPtr<ID3D12Fence>& fence, uint64_t fence_value, HANDLE fence_event);

    /**
     * Stalls thread until all command queues are empty
     */
    void FlushAllQueues();

    void Present();

    void TransitionResource(const ComPtr<ID3D12GraphicsCommandList>& command_list, const ComPtr<ID3D12Resource>& resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

    D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorByIdx(D3D12_CPU_DESCRIPTOR_HANDLE start, uint32 idx, uint32 descriptor_size);
    D3D12_CPU_DESCRIPTOR_HANDLE GetRTVDescriptorByIdx(const ComPtr<ID3D12DescriptorHeap>& descriptor_heap, uint32 idx);
    D3D12_CPU_DESCRIPTOR_HANDLE GetResourceDescriptorByIdx(const ComPtr<ID3D12DescriptorHeap>& descriptor_heap, uint32 idx);

    inline ComPtr<IDXGIFactory7> dxgi_factory;
    inline ComPtr<IDXGIAdapter4> adapter;
    inline ComPtr<ID3D12Device4> device;
    inline ComPtr<ID3D12CommandQueue> command_queue_direct;   // Can execute draw, compute & copy omnemands
    inline ComPtr<ID3D12CommandQueue> command_queue_compute;  // Can execute compute & copy commands
    inline ComPtr<ID3D12CommandQueue> command_queue_copy;     // Can only execute copy commands

    static inline constexpr int32 NUM_FRAMES_IN_FLIGHT = 2;
    inline uint64 current_frame_idx = 0;
    inline uint8 current_backbuffer_idx = 0;
    inline ComPtr<IDXGISwapChain3> swapchain;
    inline ComPtr<ID3D12DescriptorHeap> descriptor_heap_rtv;
    inline ComPtr<ID3D12DescriptorHeap> descriptor_heap_cbv_uav_srv;
    inline ComPtr<ID3D12DescriptorHeap> descriptor_heap_dsv;

    inline std::vector<ComPtr<ID3D12Resource>> backbuffers;
    inline HANDLE fence_event;
    inline ComPtr<ID3D12Fence> backbuffer_fence;
    inline std::vector<uint64> backbuffer_fence_values;

    inline std::vector<ComPtr<ID3D12CommandAllocator>> command_allocators;
    inline std::vector<ComPtr<ID3D12GraphicsCommandList>> command_lists;

    inline IRenderer* renderer = nullptr;

    inline Vec2 render_resolution = Vec2::ZERO;
    inline CD3DX12_VIEWPORT viewport;

    // Scene Data
    inline Camera camera = Camera();
}
