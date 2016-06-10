#pragma once

#include <cstdint> /* for uint32_t */
#include <cstddef> /* for size_t */

#include <vector> /* for std::vector */

/*
 * Types and macros that come from Windows
 */

typedef long HRESULT;
typedef long HMODULE;

#define S_FALSE ((HRESULT)1)
#define S_OK ((HRESULT)0)

#define MAKE_HRESULT(x, y, z) ((HRESULT) (((unsigned long)(x)<<31) | ((unsigned long)(y)<<16) | ((unsigned long)(z))) )

#define SUCCEEDED(x) ((HRESULT)(x) == S_OK)
#define FAILED(x) (!SUCCEEDED(x))

/*
 * Macros that come from D3D
 */

#define D3DERR_INVALIDCALL /* FIXME */
#define D3DERR_WASSTILLDRAWING /* FIXME */

#define D3DCOMPILE_DEBUG                          0x0001
#define D3DCOMPILE_SKIP_VALIDATION                0x0002
#define D3DCOMPILE_SKIP_OPTIMIZATION              0x0004
#define D3DCOMPILE_PACK_MATRIX_ROW_MAJOR          0x0008
#define D3DCOMPILE_AVOID_FLOW_CONTROL             0x0200
#define D3DCOMPILE_PREFER_FLOW_CONTROL            0x0400
#define D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY 0x1000

#define D3DCOMPILE_OPTIMIZATION_LEVEL0 0x4000
#define D3DCOMPILE_OPTIMIZATION_LEVEL1 0x0000
#define D3DCOMPILE_OPTIMIZATION_LEVEL2 0xc000
#define D3DCOMPILE_OPTIMIZATION_LEVEL3 0x8000

/*
 * Macros that come from D3D10
 */

#define _FACD3D10 0x87
#define MAKE_D3D10_HRESULT(x) MAKE_HRESULT(1, _FACD3D10, x)

#define D3D10_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS MAKE_D3D10_HRESULT(1)
#define D3D10_ERROR_FILE_NOT_FOUND MAKE_D3D10_HRESULT(2)

#define D3D10_SHADER_DEBUG                          0x0001
#define D3D10_SHADER_SKIP_VALIDATION                0x0002
#define D3D10_SHADER_SKIP_OPTIMIZATION              0x0004
#define D3D10_SHADER_PACK_MATRIX_ROW_MAJOR          0x0008
#define D3D10_SHADER_AVOID_FLOW_CONTROL             0x0200
#define D3D10_SHADER_PREFER_FLOW_CONTROL            0x0400
#define D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY 0x1000

#define D3D10_SHADER_OPTIMIZATION_LEVEL0 0x4000
#define D3D10_SHADER_OPTIMIZATION_LEVEL1 0x0000
#define D3D10_SHADER_OPTIMIZATION_LEVEL2 0xc000
#define D3D10_SHADER_OPTIMIZATION_LEVEL3 0x8000

/*
 * Types that come from D3D
 */

struct D3D_SHADER_MACRO
{
    char const *Name;
    char const *Definition;
};

struct ID3DInclude
{
    // FIXME: unimplemented
};

struct ID3DBlob
{
    ID3DBlob(size_t size) { m_data.resize(size); }

    void const *GetBufferPointer() const { return m_data.data(); }
    void *GetBufferPointer() { return m_data.data(); }
    size_t GetBufferSize() const { return m_data.size(); }
    void Release() { delete this; }

private:
    std::vector<uint8_t> m_data;
};

/*
 * Helper class
 */

#include <d3d4linux_impl.h>

/*
 * Functions that come from D3D
 */

static inline
HRESULT D3DCreateBlob(size_t Size, ID3DBlob **ppBlob)
{
    return d3d4linux::create_blob(Size, ppBlob);
}

static inline
HRESULT D3DCompile(void const *pSrcData,
                size_t SrcDataSize,
                char const *pFileName,
                D3D_SHADER_MACRO const *pDefines,
                ID3DInclude *pInclude,
                char const *pEntrypoint,
                char const *pTarget,
                uint32_t Flags1,
                uint32_t Flags2,
                ID3DBlob **ppCode,
                ID3DBlob **ppErrorMsgs)
{
    return d3d4linux::compile(pSrcData, SrcDataSize, pFileName, pDefines,
                              pInclude, pEntrypoint, pTarget, Flags1,
                              Flags2, ppCode, ppErrorMsgs);
}

typedef decltype(&d3d4linux::compile) pD3DCompile;

/*
 * Helper functions for Windows
 */

static inline HMODULE LoadLibrary(char const *name)
{
    return 0;
}

static void *GetProcAddress(HMODULE, char const *name)
{
    if (!strcmp(name, "D3DCompile"))
        return (void *)&d3d4linux::compile;
    if (!strcmp(name, "D3DCreateBlob"))
        return (void *)&d3d4linux::create_blob;
    return nullptr;
}

