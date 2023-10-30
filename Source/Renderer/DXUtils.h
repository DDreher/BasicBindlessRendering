#pragma once

#include <wrl/client.h>
#include "Renderer/DXErr.h"

using Microsoft::WRL::ComPtr;
// ^-  I guess using declarations are fine in this case... :x

void DXResultFailed(HRESULT result, const char* dx_call, const char* file, uint32 line);

#define DX_VERIFY(x) { HRESULT hresult = x; if(FAILED(hresult)) { DXResultFailed(hresult, #x, __FILE__, __LINE__);} }

// Safe release of COM pointers
template<typename T>
inline void SafeRelease(T& com_ptr)
{
    CHECK(com_ptr != nullptr);
    com_ptr->Release();
    com_ptr = nullptr;
}
