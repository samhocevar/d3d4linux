#pragma once

#include <cstdint> /* for uint32_t */
#include <cstddef> /* for size_t */

#include <vector> /* for std::vector */

/*
 * Types and macros that come from Windows
 */

typedef long HRESULT;
typedef long HMODULE;
typedef void const * LPCVOID;
typedef char const * LPCSTR;

typedef long GUID;
typedef GUID REFIID; /* FIXME */

#define CONST const

#define S_FALSE ((HRESULT)1)
#define S_OK ((HRESULT)0)
#define E_FAIL ((HRESULT)0x80004005)

#define SUCCEEDED(x) ((HRESULT)(x) == S_OK)
#define FAILED(x) (!SUCCEEDED(x))

#define EXCEPTION_EXECUTE_HANDLER 1

#define MAKE_HRESULT(x, y, z) ((HRESULT) (((unsigned long)(x)<<31) | ((unsigned long)(y)<<16) | ((unsigned long)(z))) )

/*
 * Enums/macros from D3D
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
 * Enums/macros from D3D10
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
 * Fake values; we don’t care
 */

#define IID_ID3D11ShaderReflection D3D4LINUX_IID_SHADER_REFLECTION

/*
 * Types that come from D3D
 */

#include <d3d4linux_enums.h>
#include <d3d4linux_types.h>

struct ID3DInclude
{
    // FIXME: unimplemented
};

struct ID3D11ShaderReflection
{
    ID3D11ShaderReflection() : m_refcount(1) {}

    // FIXME: unimplemented
    HRESULT GetDesc(D3D11_SHADER_DESC *Desc) { return S_OK; }
    HRESULT GetInputParameterDesc(uint32_t index, D3D11_SIGNATURE_PARAMETER_DESC *desc) { return S_OK; }
    HRESULT GetOutputParameterDesc(uint32_t index, D3D11_SIGNATURE_PARAMETER_DESC *desc) { return S_OK; }
    HRESULT GetResourceBindingDesc(uint32_t index, D3D11_SHADER_INPUT_BIND_DESC *desc) { return S_OK; }
    struct ID3D11ShaderReflectionConstantBuffer *GetConstantBufferByName(char const *name) { return nullptr; }

    void AddRef() { ++m_refcount; }
    void Release() { if (this && --m_refcount <= 0) delete this; }

private:
    int m_refcount;
};

struct ID3D11ShaderReflectionConstantBuffer
{
    // FIXME: unimplemented
    HRESULT GetDesc(D3D11_SHADER_BUFFER_DESC *desc) { return S_OK; }
    struct ID3D11ShaderReflectionVariable *GetVariableByIndex(uint32_t index) { return nullptr; }
    struct ID3D11ShaderReflectionVariable *GetVariableByName(char const *name) { return nullptr; }
};

struct ID3D11ShaderReflectionVariable
{
    // FIXME: unimplemented
    HRESULT GetDesc(D3D11_SHADER_VARIABLE_DESC *desc) { return S_OK; }
};

struct ID3DBlob
{
    ID3DBlob(size_t size) : m_refcount(1) { m_data.resize(size); }

    void const *GetBufferPointer() const { return m_data.data(); }
    void *GetBufferPointer() { return m_data.data(); }
    size_t GetBufferSize() const { return m_data.size(); }
    void AddRef() { ++m_refcount; }
    void Release() { if (this && --m_refcount <= 0) delete this; }

private:
    std::vector<uint8_t> m_data;
    int m_refcount;
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
HRESULT D3DCompile(void const *pSrcData, size_t SrcDataSize,
                   char const *pFileName,
                   D3D_SHADER_MACRO const *pDefines,
                   ID3DInclude *pInclude,
                   char const *pEntrypoint, char const *pTarget,
                   uint32_t Flags1, uint32_t Flags2,
                   ID3DBlob **ppCode, ID3DBlob **ppErrorMsgs)
{
    return d3d4linux::compile(pSrcData, SrcDataSize, pFileName, pDefines,
                              pInclude, pEntrypoint, pTarget, Flags1,
                              Flags2, ppCode, ppErrorMsgs);
}

static inline
HRESULT D3DDisassemble(void const *pSrcData,
                       size_t SrcDataSize,
                       uint32_t Flags,
                       char const *szComments,
                       ID3DBlob **ppDisassembly)
{
    return d3d4linux::disassemble(pSrcData, SrcDataSize, Flags,
                                  szComments, ppDisassembly);
}

static inline
HRESULT D3DReflect(void const *pSrcData,
                   size_t SrcDataSize,
                   REFIID pInterface,
                   void **ppReflector)
{
    return d3d4linux::reflect(pSrcData, SrcDataSize, pInterface, ppReflector);
}

static inline
HRESULT D3DStripShader(void const *pShaderBytecode,
                       size_t BytecodeLength,
                       uint32_t uStripFlags,
                       ID3DBlob **ppStrippedBlob)
{
    return d3d4linux::strip_shader(pShaderBytecode, BytecodeLength,
                                   uStripFlags, ppStrippedBlob);
}

/*
 * All these types do not necessarily exist in D3D, but they’re convenient
 */

typedef decltype(&d3d4linux::compile) pD3DCompile;
typedef decltype(&d3d4linux::reflect) pD3DReflect;
typedef decltype(&d3d4linux::disassemble) pD3DDisassemble;
typedef decltype(&d3d4linux::strip_shader) pD3DStripShader;

/*
 * Helper functions for Windows
 */

#undef LoadLibrary
static inline HMODULE LoadLibrary(char const *name)
{
    return 0;
}

static inline HMODULE LoadLibrary(wchar_t const *name)
{
    return 0;
}

#undef FreeLibrary
static inline void FreeLibrary(HMODULE handle)
{
}

#undef GetProcAddress
static void *GetProcAddress(HMODULE, char const *name)
{
    if (!strcmp(name, "D3DCompile"))
        return (void *)&d3d4linux::compile;
    if (!strcmp(name, "D3DReflect"))
        return (void *)&d3d4linux::reflect;
    if (!strcmp(name, "D3DDisassemble"))
        return (void *)&d3d4linux::disassemble;
    if (!strcmp(name, "D3DStripShader"))
        return (void *)&d3d4linux::strip_shader;
    if (!strcmp(name, "D3DCreateBlob"))
        return (void *)&d3d4linux::create_blob;
    return nullptr;
}

