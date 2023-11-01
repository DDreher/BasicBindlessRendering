#pragma comment(lib,"d3d12.lib")

#include "Renderer/GraphicsContext.h"

#include "d3dx12.h"
#include "d3d12sdklayers.h"

#include "Core/Window.h"
#include "Renderer/IRenderer.h"

extern IRenderer* CreateRenderer();

namespace gfx
{
    void Init(Window* window)
    {
        LOG("Initializing Graphics Context");
        CHECK(IsInitialized() == false);
        CHECK(window != nullptr);
        const HWND hwnd = static_cast<HWND>(window->GetHandle());
        CHECK(hwnd != nullptr);

#ifdef _DEBUG
        ComPtr<ID3D12Debug1> debug_interface;
        DX_VERIFY(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_interface)));
        debug_interface->EnableDebugLayer();
        debug_interface->SetEnableGPUBasedValidation(true);
#endif

        uint32 dxgi_factory_flags = 0;
#ifdef _DEBUG
        dxgi_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
        DX_VERIFY(CreateDXGIFactory2(dxgi_factory_flags, IID_PPV_ARGS(&gfx::dxgi_factory)));
        DX_VERIFY(gfx::dxgi_factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE::DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&gfx::adapter)));
        DXGI_ADAPTER_DESC adapter_desc;
        DX_VERIFY(gfx::adapter->GetDesc(&adapter_desc));
        LOG(L"Using device: {}", adapter_desc.Description);

        DX_VERIFY(D3D12CreateDevice(gfx::adapter.Get(), D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&gfx::device)));

#ifdef _DEBUG
        ComPtr<ID3D12InfoQueue> info_queue;
        DX_VERIFY(gfx::device.As(&info_queue));
        info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY::D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY::D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
        info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY::D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
#endif

        D3D12_COMMAND_QUEUE_DESC queue_desc = {};
        queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queue_desc.Type = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;
        DX_VERIFY(gfx::device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&gfx::command_queue_direct)));
        queue_desc.Type = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COMPUTE;
        DX_VERIFY(gfx::device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&gfx::command_queue_compute)));
        queue_desc.Type = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COPY;
        DX_VERIFY(gfx::device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&gfx::command_queue_copy)));

        CreateSwapchain(static_cast<uint32>(render_resolution.x), static_cast<uint32>(render_resolution.y), NUM_FRAMES_IN_FLIGHT, hwnd);

        for (int32 i = 0; i < NUM_FRAMES_IN_FLIGHT; ++i)
        {
            const D3D12_COMMAND_LIST_TYPE cmd_list_type = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;
            command_allocators.push_back(CreateCommandAllocator(cmd_list_type));
            command_lists.push_back(CreateGraphicsCommandList(command_allocators[i], cmd_list_type));
        }

        SetRenderResolution(window->GetWidth(), window->GetHeight());
        SetViewport(render_resolution.x, render_resolution.y);
        renderer = CreateRenderer();
    }

    void Shutdown()
    {
        LOG("Shutting down Graphics Context");
        CHECK(IsInitialized());

        FlushAllQueues();

        CloseHandle(fence_event);

        delete renderer;
        renderer = nullptr;

        for (auto& cmd_list : command_lists)
        {
            cmd_list.Reset();
        }

        backbuffer_fence.Reset();
        descriptor_heap_rtv.Reset();
        descriptor_heap_cbv_uav_srv.Reset();
        descriptor_heap_dsv.Reset();
        for (auto& allocator : command_allocators)
        {
            allocator.Reset();
        }

        for(auto& backbuffer : backbuffers)
        {
            backbuffer.Reset();
        }
        backbuffers.clear();

        swapchain.Reset();

        command_queue_direct.Reset();
        command_queue_compute.Reset();
        command_queue_copy.Reset();

        device.Reset();
        adapter.Reset();

        dxgi_factory.Reset();
    }

    bool IsInitialized()
    {
        return device != nullptr &&
            swapchain != nullptr &&
            //render_state_cache != nullptr &&
            renderer != nullptr;
    }

    void SetViewport(float width, float height)
    {
        CHECK(width > 0.0f);
        CHECK(height > 0.0f);
        if(viewport.Width != width || viewport.Height != height)
        {
            LOG("Set viewport - w: {} h: {}", width, height);
            viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, width, height);
        }
    }

    const D3D12_VIEWPORT& GetViewport()
    {
        return gfx::viewport;
    }

    void SetRenderResolution(uint32 width, uint32 height)
    {
        if (render_resolution.x != width || render_resolution.y != height)
        {
            render_resolution.x = static_cast<float>(width);
            render_resolution.y = static_cast<float>(height);
        }
    }

    Vec2 GetRenderResolution()
    {
        return gfx::render_resolution;
    }

    void CreateSwapchain(uint32 width, uint32 height, uint32 num_buffers, const HWND& hwnd)
    {
        DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {};
        swapchain_desc.Width = static_cast<UINT>(width);
        swapchain_desc.Height = static_cast<UINT>(height);
        swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // Not SRGB due to DXGI_SWAP_EFFECT_FLIP_DISCARD!
        // See: https://walbourn.github.io/care-and-feeding-of-modern-swapchains/
        swapchain_desc.Stereo = FALSE;
        swapchain_desc.SampleDesc = { .Count = 1, .Quality = 0 };
        swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;   // Describes surface usage and CPU access for backbuffer
        swapchain_desc.BufferCount = num_buffers;
        swapchain_desc.Scaling = DXGI_SCALING::DXGI_SCALING_STRETCH;
        swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapchain_desc.AlphaMode = DXGI_ALPHA_MODE::DXGI_ALPHA_MODE_IGNORE;
        swapchain_desc.Flags = DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
        // ^- TODO: Only set flag if hardware supports gsync or freesync

        ComPtr<IDXGISwapChain1> swapchain_1;
        DX_VERIFY(gfx::dxgi_factory->CreateSwapChainForHwnd(gfx::command_queue_direct.Get(),
            hwnd, &swapchain_desc, nullptr, nullptr, &swapchain_1));
        DX_VERIFY(swapchain_1.As(&gfx::swapchain));

        // -- Create descriptor heaps
        // ---- RTVs
        D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc = {};
        descriptor_heap_desc.NumDescriptors = num_buffers;
        descriptor_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        descriptor_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        DX_VERIFY(gfx::device->CreateDescriptorHeap(&descriptor_heap_desc, IID_PPV_ARGS(&gfx::descriptor_heap_rtv)));

        descriptor_heap_desc.NumDescriptors = 1;
        descriptor_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        descriptor_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        DX_VERIFY(gfx::device->CreateDescriptorHeap(&descriptor_heap_desc, IID_PPV_ARGS(&gfx::descriptor_heap_dsv)));

        // ---- CBVs / SRVs / UAVs
        descriptor_heap_desc.NumDescriptors = 1;
        descriptor_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        descriptor_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        DX_VERIFY(gfx::device->CreateDescriptorHeap(&descriptor_heap_desc, IID_PPV_ARGS(&gfx::descriptor_heap_cbv_uav_srv)));

        // Create RTV for each back buffer
        for (uint32 i = 0; i < num_buffers; ++i)
        {
            D3D12_CPU_DESCRIPTOR_HANDLE rtv_descriptor_handle = GetRTVDescriptorByIdx(descriptor_heap_rtv, i);
            ComPtr<ID3D12Resource> back_buffer;

            D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {};
            rtv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

            DX_VERIFY(gfx::swapchain->GetBuffer(i, IID_PPV_ARGS(&back_buffer)));
            device->CreateRenderTargetView(back_buffer.Get(), &rtv_desc, rtv_descriptor_handle);
            gfx::backbuffers.push_back(std::move(back_buffer));
        }
        CHECK(gfx::backbuffers.size() == num_buffers);

        gfx::backbuffer_fence = CreateFence();
        gfx::fence_event = CreateEventHandle();
        gfx::backbuffer_fence_values = std::vector<uint64>(num_buffers, 0);
    }

    ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE type)
    {
        ComPtr<ID3D12CommandAllocator> allocator;
        DX_VERIFY(device->CreateCommandAllocator(type, IID_PPV_ARGS(&allocator)));
        return std::move(allocator);
    }

    ComPtr<ID3D12GraphicsCommandList> CreateGraphicsCommandList(const ComPtr<ID3D12CommandAllocator>& allocator, D3D12_COMMAND_LIST_TYPE type)
    {
        ComPtr<ID3D12GraphicsCommandList> graphics_cmd_list;
        DX_VERIFY(gfx::device->CreateCommandList(0, type, allocator.Get(), nullptr, IID_PPV_ARGS(&graphics_cmd_list)));
        DX_VERIFY(graphics_cmd_list->Close());  // Close so we can reset in render loop
        return std::move(graphics_cmd_list);
    }

    ComPtr<ID3D12Fence> CreateFence()
    {
        static constexpr int INITIAL_VALUE = 0;
        ComPtr<ID3D12Fence> fence;
        DX_VERIFY(gfx::device->CreateFence(INITIAL_VALUE, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
        return std::move(fence);
    }

    void Signal(const ComPtr<ID3D12CommandQueue>& queue, const ComPtr<ID3D12Fence>& fence, uint64 signal_value)
    {
        // Signals a fence with the given value once the command queue reached the signal
        DX_VERIFY(queue->Signal(fence.Get(), signal_value));
    }

    HANDLE CreateEventHandle()
    {
        HANDLE fence_event;
        fence_event = CreateEvent(NULL, FALSE, FALSE, NULL);
        CHECK(fence_event);
        return fence_event;
    }

    void WaitForFence(const ComPtr<ID3D12Fence>& fence, uint64 value, HANDLE fence_event)
    {
        CHECK(fence_event);
        if (fence->GetCompletedValue() < value)
        {
            // Stall thread until fence reached the given value
            DX_VERIFY(fence->SetEventOnCompletion(value, fence_event));
            WaitForSingleObject(fence_event, INFINITE);
        }
    }

    void FlushQueue(const ComPtr<ID3D12CommandQueue>& queue, const ComPtr<ID3D12Fence>& fence, uint64_t fence_value, HANDLE fence_event)
    {
        Signal(queue, fence, fence_value);
        WaitForFence(fence, fence_value, fence_event);
    }

    void FlushAllQueues()
    {
        ComPtr<ID3D12Fence> flush_fence = CreateFence();
        FlushQueue(gfx::command_queue_direct, flush_fence, 0, fence_event);
        FlushQueue(gfx::command_queue_compute, flush_fence, 1, fence_event);
        FlushQueue(gfx::command_queue_copy, flush_fence, 2, fence_event);
        WaitForFence(flush_fence, 2, fence_event);
    }

    void Present()
    {
        DX_VERIFY(swapchain->Present(0 /* no vsync*/, DXGI_PRESENT_ALLOW_TEARING));

        // Enqueue signal so we can check when rendering is complete and we can reuse the resources
        const uint64 fence_value = ++current_frame_idx;
        Signal(gfx::command_queue_direct, gfx::backbuffer_fence, fence_value);
        gfx::backbuffer_fence_values[gfx::current_backbuffer_idx] = fence_value;

        // When FLIP_DISCARD, we can't rely on sequential backbuffer indices, so we have to query the swapchain.
        current_backbuffer_idx = gfx::swapchain->GetCurrentBackBufferIndex();

        // Stall until we can be sure that we can access the resources accessed by the next back buffer
        WaitForFence(gfx::backbuffer_fence, gfx::backbuffer_fence_values[gfx::current_backbuffer_idx], gfx::fence_event);
    }

    void TransitionResource(const ComPtr<ID3D12GraphicsCommandList>& command_list, const ComPtr<ID3D12Resource>& resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
    {
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), before, after);
        command_list->ResourceBarrier(1, &barrier);
    }

    D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorByIdx(D3D12_CPU_DESCRIPTOR_HANDLE start, uint32 idx, uint32 descriptor_size)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE out = start;
        out.ptr += idx * descriptor_size;
        return out;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE GetRTVDescriptorByIdx(const ComPtr<ID3D12DescriptorHeap>& descriptor_heap, uint32 idx)
    {
        CHECK(descriptor_heap);
        static const uint32 SIZE_DESCRIPTOR = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        // ^- size of descriptor is vendor specific, we have to query
        return GetDescriptorByIdx(descriptor_heap->GetCPUDescriptorHandleForHeapStart(), idx, SIZE_DESCRIPTOR);
    }

    D3D12_CPU_DESCRIPTOR_HANDLE GetResourceDescriptorByIdx(const ComPtr<ID3D12DescriptorHeap>& descriptor_heap, uint32 idx)
    {
        CHECK(descriptor_heap);
        static const uint32 SIZE_DESCRIPTOR = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        // ^- size of descriptor is vendor specific, we have to query
        return GetDescriptorByIdx(descriptor_heap->GetCPUDescriptorHandleForHeapStart(), idx, SIZE_DESCRIPTOR);
    }
}
