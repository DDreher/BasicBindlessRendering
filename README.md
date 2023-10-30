# DirectX12 - Basic Bindless Rendering

Sometimes Computer Graphics seems like black magic. There are a few tutorials for fundamentals but often I find myself longing for very basic projects to analyze and eventually grasp the underlying concepts of more advanced rendering techniques.

This project aims to provide such a basic project to showcases how to achieve **bindless rendering** with **DirectX 12** and **Shader Model 6.6**.

**Bindless rendering** refers to a technique with which we can directly access descriptors in a shader via an index into global descriptor heaps (`ResourceDescriptorHeap` or `SamplerDescriptorHeap`).

This vastly simplifies descriptor and pipeline state management:
* No need to bind Resource Views ( aka `descriptors`, e.g. constant buffer views, shader resource views, ... )
* No need to manage root signature permutations.
* No need to manage Vertex Layouts. Vertex attributes are directly accessed by indexing into a bindless `SRV` via `VertexID`. All we need to do is set the Index Buffer.

```
cbuffer PerDrawConstants : register(b0, space0)
{
    uint32 pos_buffer_index;
};

VSOutput Main(uint vertex_id : SV_VertexID)
{
    StructuredBuffer<float4> pos_buffer = ResourceDescriptorHeap[pos_buffer_index];
    float4 pos = pos_buffer[vertex_id];

    // ...
}
```

If you just want to explore how bindless rendering works, I encourage you to check out the following files:
`Source/Renderer.cpp` and `Source/Shaders/bindless_vs.hlsl`.

## Disclaimer

The code inside this repository was only written for learning purposes. I never really tried to make it beautiful, easy to read, performant or 100% correct. In the end, all I wanted to do is to explore the API. Now I share my code so other people doing the same may be inspired by my journey.

## Prerequisites

* Windows 11 (may also work with Windows 10 though)
* A modern GPU (tested on RTX 3060 ti)
* Visual Studio 2022
* Windows SDK (tested with `10.0.226201.0`, installed via VS2022)

## How To Build

* Execute `GenerateProjectFiles.bat`. This will invoke [Premake5](https://premake.github.io/) to generate a solution for VS2022.
    * The DirectX12 Agility SDK will be automatically fetched via Nuget.
* Open the generated solution (`BasicBindless.sln`)
* Build and run the `BasicBindless` project in your desired configuration, e.g. `Debug` or `Release`.

## Controls

* `WASD` - Move forward / left / backward / right
* `Space` - Move up
* `Ctrl` - Move down
* `Hold right click` - Change view direction

## Dependencies

* [Premake5](https://premake.github.io/)
* [SDL2](https://www.libsdl.org/)
* [Spdlog](https://github.com/gabime/spdlog)

## Further Resources

* [Ray Tracing Gems II (Chapter 17)](https://developer.nvidia.com/blog/ray-tracing-gems-ii-available-today-as-free-digital-download/)
* [Wicked Engine DevBlog - Bindless Descriptors](https://wickedengine.net/2021/04/06/bindless-descriptors/)
* [Alex Tardif - Binding Bindlessly](https://alextardif.com/Bindless.html)
* [DirectX Developer Blog - Announcing Shader Model 6.6](https://devblogs.microsoft.com/directx/hlsl-shader-model-6-6/)
* [Tarun Ramaswamy - Bindless Rendering in DirectX12 and SM6.6](https://rtarun9.github.io/blogs/bindless_rendering/)
