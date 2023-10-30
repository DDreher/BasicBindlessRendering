#include "Renderer.h"

#if _RENDER_DEBUG
#include <dxgidebug.h>
#endif

#include "Core/Application.h"
#include "Core/FileIO.h"
#include "Renderer/Camera.h"
#include "Renderer/GraphicsContext.h"

Renderer::Renderer()
{
    const uint8 backbuffer_idx = gfx::current_backbuffer_idx;
    const ComPtr<ID3D12CommandAllocator>& command_allocator = gfx::command_allocators[backbuffer_idx];
    const ComPtr<ID3D12GraphicsCommandList>& command_list = gfx::command_lists[backbuffer_idx];

    command_allocator->Reset();
    command_list->Reset(command_allocator.Get(), nullptr);

    // Global Descriptor Heaps
    for (int i = 0; i < gfx::NUM_FRAMES_IN_FLIGHT; ++i)
    {
        D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc = {};
        descriptor_heap_desc.NumDescriptors = 512;
        descriptor_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        descriptor_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        DX_VERIFY(gfx::device->CreateDescriptorHeap(&descriptor_heap_desc, IID_PPV_ARGS(&descriptor_heaps_cbv_uav_srv[i])));
    }

    // Create index buffer
    uint32 index_buffer_size = (uint32)(sizeof(uint32) * CubeMeshData::INDICES.size());

    // Create default heap to hold index buffer
    {
        const D3D12_HEAP_PROPERTIES heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        const CD3DX12_RESOURCE_DESC buffer_desc = CD3DX12_RESOURCE_DESC::Buffer(index_buffer_size);
        gfx::device->CreateCommittedResource(
            &heap_properties,
            D3D12_HEAP_FLAG_NONE,
            &buffer_desc,
            D3D12_RESOURCE_STATE_COPY_DEST, // Start in copy destination state
            nullptr,
            IID_PPV_ARGS(&index_buffer_));
        index_buffer_->SetName(L"Index Buffer Resource Heap");
    }

    // Create upload heap to upload index buffer
    ComPtr<ID3D12Resource> index_buffer_upload_heap;
    {
        const D3D12_HEAP_PROPERTIES heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        const CD3DX12_RESOURCE_DESC buffer_desc = CD3DX12_RESOURCE_DESC::Buffer(index_buffer_size);
        gfx::device->CreateCommittedResource(
            &heap_properties, // upload heap
            D3D12_HEAP_FLAG_NONE, // no flags
            &buffer_desc, // resource description for a buffer
            D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
            nullptr,
            IID_PPV_ARGS(&index_buffer_upload_heap));
        index_buffer_upload_heap->SetName(L"Upload Index Buffer Resource Heap");
    }

    // Copy index buffer to upload heap and then to default heap
    {
        D3D12_SUBRESOURCE_DATA index_buffer_data = {};
        index_buffer_data.pData = (void*)CubeMeshData::INDICES.data(); // pointer to our index array
        index_buffer_data.RowPitch = index_buffer_size;
        index_buffer_data.SlicePitch = index_buffer_size;
        UpdateSubresources(command_list.Get(), index_buffer_.Get(), index_buffer_upload_heap.Get(), 0, 0, 1, &index_buffer_data);
    }
    gfx::TransitionResource(command_list, index_buffer_, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);

    // Create index buffer view
    index_buffer_view_.BufferLocation = index_buffer_->GetGPUVirtualAddress();
    index_buffer_view_.Format = DXGI_FORMAT_R32_UINT;
    index_buffer_view_.SizeInBytes = index_buffer_size;

    // -- Create Vertex Buffers
    // --- Pos
    uint32 pos_buffer_size = (uint32)(sizeof(Vec4) * CubeMeshData::POS.size());

    // Create default heap to hold buffer
    {
        const D3D12_HEAP_PROPERTIES heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        const CD3DX12_RESOURCE_DESC buffer_desc = CD3DX12_RESOURCE_DESC::Buffer(pos_buffer_size);
        gfx::device->CreateCommittedResource(
            &heap_properties,
            D3D12_HEAP_FLAG_NONE,
            &buffer_desc,
            D3D12_RESOURCE_STATE_COPY_DEST, // Start in copy destination state
            nullptr,
            IID_PPV_ARGS(&vertex_pos_buffer_));
        vertex_pos_buffer_->SetName(L"Vertex Pos Buffer");
    }

    // Create upload heap to upload index buffer
    ComPtr<ID3D12Resource> pos_upload_heap;
    {
        const D3D12_HEAP_PROPERTIES heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        const CD3DX12_RESOURCE_DESC buffer_desc = CD3DX12_RESOURCE_DESC::Buffer(pos_buffer_size);
        gfx::device->CreateCommittedResource(
            &heap_properties, // upload heap
            D3D12_HEAP_FLAG_NONE, // no flags
            &buffer_desc, // resource description for a buffer
            D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
            nullptr,
            IID_PPV_ARGS(&pos_upload_heap));
        pos_upload_heap->SetName(L"Upload Vertex Pos Buffer");
    }

    // Copy vertex buffer to upload heap and then to default heap
    {
        D3D12_SUBRESOURCE_DATA buffer_data = {};
        buffer_data.pData = reinterpret_cast<BYTE*>(CubeMeshData::POS.data()); // pointer to our index array
        buffer_data.RowPitch = pos_buffer_size;
        buffer_data.SlicePitch = pos_buffer_size;
        UpdateSubresources(command_list.Get(), vertex_pos_buffer_.Get(), pos_upload_heap.Get(), 0, 0, 1, &buffer_data);
    }
    gfx::TransitionResource(command_list, vertex_pos_buffer_, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

    // Create SRVs
    for (int i = 0; i < gfx::NUM_FRAMES_IN_FLIGHT; ++i)
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC view_desc = {};
        view_desc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
        view_desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        view_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        view_desc.Buffer = {
            .FirstElement = 0,
            .NumElements = (uint32)CubeMeshData::POS.size(),
            .StructureByteStride = sizeof(Vec4),
            .Flags = D3D12_BUFFER_SRV_FLAGS::D3D12_BUFFER_SRV_FLAG_NONE
        };
        const D3D12_CPU_DESCRIPTOR_HANDLE destination_handle = gfx::GetResourceDescriptorByIdx(descriptor_heaps_cbv_uav_srv[i], i);
        gfx::device->CreateShaderResourceView(vertex_pos_buffer_.Get(), &view_desc, destination_handle);
    }

    // --- UVs
    uint32 uv_buffer_size = (uint32)(sizeof(Vec2) * CubeMeshData::UVS.size());

    // Create default heap to hold buffer
    {
        const D3D12_HEAP_PROPERTIES heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        const CD3DX12_RESOURCE_DESC buffer_desc = CD3DX12_RESOURCE_DESC::Buffer(uv_buffer_size);
        gfx::device->CreateCommittedResource(
            &heap_properties,
            D3D12_HEAP_FLAG_NONE,
            &buffer_desc,
            D3D12_RESOURCE_STATE_COPY_DEST, // Start in copy destination state
            nullptr,
            IID_PPV_ARGS(&vertex_uv_buffer_));
        vertex_uv_buffer_->SetName(L"Vertex UV Buffer");
    }

    // Create upload heap to upload index buffer
    ComPtr<ID3D12Resource> uv_upload_heap;
    {
        const D3D12_HEAP_PROPERTIES heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        const CD3DX12_RESOURCE_DESC buffer_desc = CD3DX12_RESOURCE_DESC::Buffer(uv_buffer_size);
        gfx::device->CreateCommittedResource(
            &heap_properties, // upload heap
            D3D12_HEAP_FLAG_NONE, // no flags
            &buffer_desc, // resource description for a buffer
            D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
            nullptr,
            IID_PPV_ARGS(&uv_upload_heap));
        uv_upload_heap->SetName(L"Upload Vertex UV Buffer");
    }

    // Copy buffer to upload heap and then to default heap
    {
        D3D12_SUBRESOURCE_DATA buffer_data = {};
        buffer_data.pData = reinterpret_cast<uint8*>(CubeMeshData::UVS.data()); // pointer to our index array
        buffer_data.RowPitch = uv_buffer_size;
        buffer_data.SlicePitch = uv_buffer_size;
        UpdateSubresources(command_list.Get(), vertex_uv_buffer_.Get(), uv_upload_heap.Get(), 0, 0, 1, &buffer_data);
    }
    gfx::TransitionResource(command_list, vertex_uv_buffer_, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

    // Create SRVs
    for (int i = 0; i < gfx::NUM_FRAMES_IN_FLIGHT; ++i)
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC view_desc = {};
        view_desc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
        view_desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        view_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        view_desc.Buffer = {
            .FirstElement = 0,
            .NumElements = (uint32)CubeMeshData::UVS.size(),
            .StructureByteStride = sizeof(Vec2),
            .Flags = D3D12_BUFFER_SRV_FLAGS::D3D12_BUFFER_SRV_FLAG_NONE
        };
        const D3D12_CPU_DESCRIPTOR_HANDLE destination_handle = gfx::GetResourceDescriptorByIdx(descriptor_heaps_cbv_uav_srv[i], i + gfx::NUM_FRAMES_IN_FLIGHT);
        gfx::device->CreateShaderResourceView(vertex_uv_buffer_.Get(), &view_desc, destination_handle);
    }

    // -- Scene Data Constant Buffer
    for (int i = 0; i < gfx::NUM_FRAMES_IN_FLIGHT; ++i)
    {
        const D3D12_HEAP_PROPERTIES heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        const CD3DX12_RESOURCE_DESC buffer_desc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(CBufferSceneData));
        gfx::device->CreateCommittedResource(
            &heap_properties,
            D3D12_HEAP_FLAG_NONE,
            &buffer_desc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&cbuffer_heaps[i]));
        cbuffer_heaps[i]->SetName(L"CBuffer Scene Data" + i);
        static const CD3DX12_RANGE ZERO_READ_RANGE(0, 0); // no CPU read
        DX_VERIFY(cbuffer_heaps[i]->Map(0, &ZERO_READ_RANGE, &cbuffer_gpu_ptrs[i]));

        D3D12_CONSTANT_BUFFER_VIEW_DESC view_desc = {};
        view_desc.BufferLocation = cbuffer_heaps[i]->GetGPUVirtualAddress();
        view_desc.SizeInBytes = (uint32) MathUtils::AlignToBytes(sizeof(CBufferSceneData), 256);    // CB size is required to be 256-byte aligned.
        const D3D12_CPU_DESCRIPTOR_HANDLE destination_handle = gfx::GetResourceDescriptorByIdx(descriptor_heaps_cbv_uav_srv[i], i + gfx::NUM_FRAMES_IN_FLIGHT * 2);
        gfx::device->CreateConstantBufferView(&view_desc, destination_handle);
    }

    // -- Shaders
    String vs_path = "Assets/Shaders/bindless_vs.cso";
    std::vector<uint8> vs_data = FileIO::ReadFile(vs_path);
    D3D12_SHADER_BYTECODE vs_bytecode = { vs_data.data(), vs_data.size() };

    String ps_path = "Assets/Shaders/bindless_ps.cso";
    std::vector<uint8> ps_data = FileIO::ReadFile(ps_path);
    D3D12_SHADER_BYTECODE ps_bytecode = { ps_data.data(), ps_data.size() };

    // -- Root Signature
    D3D12_ROOT_PARAMETER root_parameters[1];
    root_parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    root_parameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    root_parameters[0].Constants.ShaderRegister = 0;
    root_parameters[0].Constants.RegisterSpace = 0;
    root_parameters[0].Constants.Num32BitValues = sizeof(PerDrawConstants) / sizeof(uint32);

    CD3DX12_ROOT_SIGNATURE_DESC root_signature_desc; 
    const D3D12_ROOT_SIGNATURE_FLAGS root_signature_flags = D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;
    root_signature_desc.Init(ARRAYSIZE(root_parameters), root_parameters, 0, nullptr, root_signature_flags);
    DX_VERIFY(D3D12SerializeRootSignature(&root_signature_desc, D3D_ROOT_SIGNATURE_VERSION_1, &serialized_root_signature, nullptr));
    DX_VERIFY(gfx::device->CreateRootSignature(0, serialized_root_signature->GetBufferPointer(), serialized_root_signature->GetBufferSize(), IID_PPV_ARGS(&root_signature)));

    // -- PSO
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};
    pso_desc.pRootSignature = root_signature.Get();
    pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pso_desc.VS = vs_bytecode;
    pso_desc.PS = ps_bytecode;
    pso_desc.NumRenderTargets = 1;  // Just the backbuffer
    pso_desc.RTVFormats[0] = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    pso_desc.DSVFormat = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
    pso_desc.SampleDesc = { .Count = 1, .Quality = 0 }; // must be the same sample description as the swapchain and depth/stencil buffer
    pso_desc.SampleMask = 0xffffffff;                   // sample mask has to do with multi-sampling. 0xffffffff means point sampling is done
    pso_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pso_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    DX_VERIFY(gfx::device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&pso)));

    // -- Finalize command list & Submit
    DX_VERIFY(command_list->Close());
    ID3D12CommandList* const submitted_command_lists[] = { command_list.Get() };
    gfx::command_queue_direct->ExecuteCommandLists(_countof(submitted_command_lists), submitted_command_lists);
    gfx::FlushAllQueues();

    // -- Create depth buffer
    D3D12_VIEWPORT viewport = gfx::GetViewport();
    RecreateDepthBuffer(static_cast<int32>(viewport.Width), static_cast<int32>(viewport.Height));

    // -- Misc scene setup
    camera.SetPosition(Vec3(0.0f, 0.0f, -10.0f));
    camera.LookAt(Vec3::ZERO);
}

void Renderer::Render()
{
    camera.Update();

    const uint8 backbuffer_idx = gfx::current_backbuffer_idx;

    const ComPtr<ID3D12CommandAllocator>& command_allocator = gfx::command_allocators[backbuffer_idx];
    const ComPtr<ID3D12GraphicsCommandList>& command_list = gfx::command_lists[backbuffer_idx];

    const ComPtr<ID3D12Resource>& backbuffer_rtv = gfx::backbuffers[backbuffer_idx];
    D3D12_CPU_DESCRIPTOR_HANDLE backbuffer_rtv_handle = gfx::GetRTVDescriptorByIdx(gfx::descriptor_heap_rtv, backbuffer_idx);
    D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle = gfx::GetRTVDescriptorByIdx(gfx::descriptor_heap_dsv, 0);

    command_allocator->Reset();
    command_list->Reset(command_allocator.Get(), nullptr);

    // -- Clear
    {
        static constexpr float CLEAR_COLOR[4] = { 100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f, 255.0f / 255.0f };
        gfx::TransitionResource(command_list, backbuffer_rtv, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        command_list->ClearRenderTargetView(backbuffer_rtv_handle, CLEAR_COLOR, 0, nullptr /* clear entire rtv */);
        command_list->ClearDepthStencilView(dsv_handle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    }

    // -- Setup Pipeline State
    {
        command_list->SetPipelineState(pso.Get());
        command_list->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);   // Same as in PSO
        command_list->IASetIndexBuffer(&index_buffer_view_);

        D3D12_VIEWPORT viewport = gfx::GetViewport();
        command_list->RSSetViewports(1, &viewport);
        static D3D12_RECT SCISSOR_RECT = { 0, 0, static_cast<LONG>(viewport.Width), static_cast<LONG>(viewport.Height) };
        command_list->RSSetScissorRects(1, &SCISSOR_RECT); // Have to set in DX12

        static constexpr bool IS_CONTIGUOUS_ARRAY = false;
        command_list->OMSetRenderTargets(1, &backbuffer_rtv_handle, IS_CONTIGUOUS_ARRAY, &dsv_handle);
    }

    // Set Descriptor heaps for each command list
    // These have to be set before Root Signature!
    ID3D12DescriptorHeap* descriptor_heaps[] = { descriptor_heaps_cbv_uav_srv[backbuffer_idx].Get() };
    command_list->SetDescriptorHeaps(ARRAYSIZE(descriptor_heaps), descriptor_heaps);

    command_list->SetGraphicsRootSignature(root_signature.Get());   // Same as in PSO

    // -- Update Resources
    {
        per_draw_constants_[backbuffer_idx].position_buffer_idx = backbuffer_idx;
        per_draw_constants_[backbuffer_idx].uv_buffer_idx = backbuffer_idx + gfx::NUM_FRAMES_IN_FLIGHT;
        per_draw_constants_[backbuffer_idx].scene_cbuffer_idx = backbuffer_idx + gfx::NUM_FRAMES_IN_FLIGHT * 2;
        command_list->SetGraphicsRoot32BitConstants(0, sizeof(PerDrawConstants) / sizeof(uint32), &per_draw_constants_[backbuffer_idx], 0u);

        // Scene Data
        cbuffer.wvp = Mat4::IDENTITY * camera.GetViewProjection();
        memcpy(cbuffer_gpu_ptrs[backbuffer_idx], &cbuffer, sizeof(cbuffer));
    }

    // -- Draw
    {
        command_list->DrawIndexedInstanced(static_cast<uint32>(CubeMeshData::INDICES.size()), 1, 0, 0, 0);
    }
}

void Renderer::Present()
{
    CHECK(gfx::IsInitialized());
    const uint8 backbuffer_idx = gfx::current_backbuffer_idx;
    const ComPtr<ID3D12Resource>& backbuffer_rtv = gfx::backbuffers[backbuffer_idx];
    const ComPtr<ID3D12GraphicsCommandList>& command_list = gfx::command_lists[backbuffer_idx];

    gfx::TransitionResource(command_list, backbuffer_rtv, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

    // Finalize command list
    DX_VERIFY(command_list->Close());

    // Submit the work to the GPU
    ID3D12CommandList* const submitted_command_lists[] = { command_list.Get() };
    gfx::command_queue_direct->ExecuteCommandLists(_countof(submitted_command_lists), submitted_command_lists);

    gfx::Present();
}

void Renderer::RecreateDepthBuffer(int32 width, int32 height)
{
    gfx::FlushAllQueues();

    width = std::max(1, width);     // Could be 0 when minimized, but 0 is invalid
    height = std::max(1, height);

    D3D12_CLEAR_VALUE clear_value = {};
    clear_value.Format = DXGI_FORMAT_D32_FLOAT;
    clear_value.DepthStencil = { 1.0f, 0 };

    CD3DX12_HEAP_PROPERTIES heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_RESOURCE_DESC resource_desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

    DX_VERIFY(gfx::device->CreateCommittedResource(
        &heap_properties,
        D3D12_HEAP_FLAG_NONE,
        &resource_desc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clear_value,
        IID_PPV_ARGS(&depth_buffer_)
    ));

    D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};
    dsv_desc.Format = DXGI_FORMAT_D32_FLOAT;
    dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsv_desc.Texture2D.MipSlice = 0;
    dsv_desc.Flags = D3D12_DSV_FLAG_NONE;
    gfx::device->CreateDepthStencilView(depth_buffer_.Get(), &dsv_desc, gfx::descriptor_heap_dsv->GetCPUDescriptorHandleForHeapStart());
}

IRenderer* CreateRenderer()
{
    return new Renderer();
}
