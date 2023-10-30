#pragma comment(lib, "dxguid.lib")

#include "Renderer/DXUtils.h"

void DXResultFailed(HRESULT result, const char* dx_call, const char* file, uint32 line)
{
    const char* error_code = DXGetErrorString(result);
    char error_desc[512];
    DXGetErrorDescription(result, error_desc, sizeof(error_desc));
    LOG_ERROR("DX11 Call Failed!\n [Location] {}:{}\n [Code] {} \n [Status] {}\n [Msg] {}", file, line, dx_call, error_code, error_desc);
    abort();
}
