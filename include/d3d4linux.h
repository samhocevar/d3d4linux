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
 * Enums/macros from D3D11
 */

typedef enum _D3DCOMPILER_STRIP_FLAGS
{
    D3DCOMPILER_STRIP_REFLECTION_DATA = 0x00000001,
    D3DCOMPILER_STRIP_DEBUG_INFO      = 0x00000002,
    D3DCOMPILER_STRIP_TEST_BLOBS      = 0x00000004,
    D3DCOMPILER_STRIP_FORCE_DWORD     = 0x7fffffff,
}
D3DCOMPILER_STRIP_FLAGS;

typedef enum _D3D_NAME
{
    D3D_NAME_UNDEFINED = 0,
    D3D_NAME_POSITION = 1,
    D3D_NAME_CLIP_DISTANCE = 2,
    D3D_NAME_CULL_DISTANCE = 3,
    D3D_NAME_RENDER_TARGET_ARRAY_INDEX = 4,
    D3D_NAME_VIEWPORT_ARRAY_INDEX = 5,
    D3D_NAME_VERTEX_ID = 6,
    D3D_NAME_PRIMITIVE_ID = 7,
    D3D_NAME_INSTANCE_ID = 8,
    D3D_NAME_IS_FRONT_FACE = 9,
    D3D_NAME_SAMPLE_INDEX = 10,
    D3D_NAME_FINAL_QUAD_EDGE_TESSFACTOR = 11,
    D3D_NAME_FINAL_QUAD_INSIDE_TESSFACTOR = 12,
    D3D_NAME_FINAL_TRI_EDGE_TESSFACTOR = 13,
    D3D_NAME_FINAL_TRI_INSIDE_TESSFACTOR = 14,
    D3D_NAME_FINAL_LINE_DETAIL_TESSFACTOR = 15,
    D3D_NAME_FINAL_LINE_DENSITY_TESSFACTOR = 16,
    D3D_NAME_TARGET = 64,
    D3D_NAME_DEPTH = 65,
    D3D_NAME_COVERAGE = 66,
    D3D_NAME_DEPTH_GREATER_EQUAL = 67,
    D3D_NAME_DEPTH_LESS_EQUAL = 68,
    D3D10_NAME_UNDEFINED = 0,
    D3D10_NAME_POSITION = 1,
    D3D10_NAME_CLIP_DISTANCE = 2,
    D3D10_NAME_CULL_DISTANCE = 3,
    D3D10_NAME_RENDER_TARGET_ARRAY_INDEX = 4,
    D3D10_NAME_VIEWPORT_ARRAY_INDEX = 5,
    D3D10_NAME_VERTEX_ID = 6,
    D3D10_NAME_PRIMITIVE_ID = 7,
    D3D10_NAME_INSTANCE_ID = 8,
    D3D10_NAME_IS_FRONT_FACE = 9,
    D3D10_NAME_SAMPLE_INDEX = 10,
    D3D11_NAME_FINAL_QUAD_EDGE_TESSFACTOR = 11,
    D3D11_NAME_FINAL_QUAD_INSIDE_TESSFACTOR = 12,
    D3D11_NAME_FINAL_TRI_EDGE_TESSFACTOR = 13,
    D3D11_NAME_FINAL_TRI_INSIDE_TESSFACTOR = 14,
    D3D11_NAME_FINAL_LINE_DETAIL_TESSFACTOR = 15,
    D3D11_NAME_FINAL_LINE_DENSITY_TESSFACTOR = 16,
    D3D10_NAME_TARGET = 64,
    D3D10_NAME_DEPTH = 65,
    D3D10_NAME_COVERAGE = 66,
    D3D11_NAME_DEPTH_GREATER_EQUAL = 67,
    D3D11_NAME_DEPTH_LESS_EQUAL = 68,
}
D3D_NAME;

typedef enum _D3D_REGISTER_COMPONENT_TYPE
{
    D3D_REGISTER_COMPONENT_UNKNOWN = 0,
    D3D_REGISTER_COMPONENT_UINT32 = 1,
    D3D_REGISTER_COMPONENT_SINT32 = 2,
    D3D_REGISTER_COMPONENT_FLOAT32 = 3,

    D3D10_REGISTER_COMPONENT_UNKNOWN = 0,
    D3D10_REGISTER_COMPONENT_UINT32 = 1,
    D3D10_REGISTER_COMPONENT_SINT32 = 2,
    D3D10_REGISTER_COMPONENT_FLOAT32 = 3,
}
D3D_REGISTER_COMPONENT_TYPE;

typedef enum _D3D_SHADER_INPUT_TYPE
{
    D3D_SIT_CBUFFER = 0,
    D3D_SIT_TBUFFER = 1,
    D3D_SIT_TEXTURE = 2,
    D3D_SIT_SAMPLER = 3,
    D3D_SIT_UAV_RWTYPED = 4,
    D3D_SIT_STRUCTURED = 5,
    D3D_SIT_UAV_RWSTRUCTURED = 6,
    D3D_SIT_BYTEADDRESS = 7,
    D3D_SIT_UAV_RWBYTEADDRESS = 8,
    D3D_SIT_UAV_APPEND_STRUCTURED = 9,
    D3D_SIT_UAV_CONSUME_STRUCTURED = 10,
    D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER = 11,

    D3D10_SIT_CBUFFER = 0,
    D3D10_SIT_TBUFFER = 1,
    D3D10_SIT_TEXTURE = 2,
    D3D10_SIT_SAMPLER = 3,
    D3D11_SIT_UAV_RWTYPED = 4,
    D3D11_SIT_STRUCTURED = 5,
    D3D11_SIT_UAV_RWSTRUCTURED = 6,
    D3D11_SIT_BYTEADDRESS = 7,
    D3D11_SIT_UAV_RWBYTEADDRESS = 8,
    D3D11_SIT_UAV_APPEND_STRUCTURED = 9,
    D3D11_SIT_UAV_CONSUME_STRUCTURED = 10,
    D3D11_SIT_UAV_RWSTRUCTURED_WITH_COUNTER = 11,
}
D3D_SHADER_INPUT_TYPE;

typedef enum _D3D_SHADER_VARIABLE_FLAGS
{
  D3D_SVF_USERPACKED             = 1,
  D3D_SVF_USED                   = 2,
  D3D_SVF_INTERFACE_POINTER      = 4,
  D3D_SVF_INTERFACE_PARAMETER    = 8,

  D3D10_SVF_USERPACKED           = D3D_SVF_USERPACKED,
  D3D10_SVF_USED                 = D3D_SVF_USED,
  D3D11_SVF_INTERFACE_POINTER    = D3D_SVF_INTERFACE_POINTER,
  D3D11_SVF_INTERFACE_PARAMETER  = D3D_SVF_INTERFACE_PARAMETER,

  D3D_SVF_FORCE_DWORD            = 0x7fffffff,
}
D3D_SHADER_VARIABLE_FLAGS;

/*
 * Fake values; we don’t care
 */

#define IID_ID3D11ShaderReflection 0x0001

/*
 * Types that come from D3D
 */

struct D3D_SHADER_MACRO
{
    char const *Name;
    char const *Definition;
};

struct D3D_SHADER_DATA
{
    void const *pBytecode;
    size_t BytecodeLength;
};

struct D3D11_SIGNATURE_PARAMETER_DESC
{
    char const *SemanticName;
    uint32_t SemanticIndex;
    uint32_t Register;
    D3D_NAME SystemValueType;
    D3D_REGISTER_COMPONENT_TYPE ComponentType;
    uint8_t Mask;
    uint8_t ReadWriteMask;
    uint32_t Stream;
};

struct D3D11_SHADER_INPUT_BIND_DESC
{
    char const *Name;
    D3D_SHADER_INPUT_TYPE Type;
    uint32_t BindPoint;
    uint32_t BindCount;
    uint32_t uFlags;
    //D3D_RESOURCE_RETURN_TYPE ReturnType;
    //D3D_SRV_DIMENSION Dimension;
    uint32_t NumSamples;
};

struct D3D11_SHADER_DESC
{
    uint32_t Version;
    char const *Creator;
    uint32_t Flags;
    uint32_t ConstantBuffers;
    uint32_t BoundResources;
    uint32_t InputParameters;
    uint32_t OutputParameters;
    uint32_t InstructionCount;
    uint32_t TempRegisterCount;
    uint32_t TempArrayCount;
    uint32_t DefCount;
    uint32_t DclCount;
    uint32_t TextureNormalInstructions;
    uint32_t TextureLoadInstructions;
    uint32_t TextureCompInstructions;
    uint32_t TextureBiasInstructions;
    uint32_t TextureGradientInstructions;
    uint32_t FloatInstructionCount;
    uint32_t IntInstructionCount;
    uint32_t UintInstructionCount;
    uint32_t StaticFlowControlCount;
    uint32_t DynamicFlowControlCount;
    uint32_t MacroInstructionCount;
    uint32_t ArrayInstructionCount;
    uint32_t CutInstructionCount;
    uint32_t EmitInstructionCount;
    //D3D_PRIMITIVE_TOPOLOGY GSOutputTopology;
    uint32_t GSMaxOutputVertexCount;
    //D3D_PRIMITIVE InputPrimitive;
    uint32_t PatchConstantParameters;
    uint32_t cGSInstanceCount;
    uint32_t cControlPoints;
    //D3D_TESSELLATOR_OUTPUT_PRIMITIVE HSOutputPrimitive;
    //D3D_TESSELLATOR_PARTITIONING HSPartitioning;
    //D3D_TESSELLATOR_DOMAIN TessellatorDomain;
    uint32_t cBarrierInstructions;
    uint32_t cInterlockedInstructions;
    uint32_t cTextureStoreInstructions;
};

struct D3D11_SHADER_TYPE_DESC
{
    // FIXME: unimplemented
};

struct D3D11_SHADER_VARIABLE_DESC
{
    char const *Name;
    uint32_t StartOffset;
    uint32_t Size;
    uint32_t uFlags;
    void *DefaultValue;
    uint32_t StartTexture;
    uint32_t TextureSize;
    uint32_t StartSampler;
    uint32_t SamplerSize;
};

struct D3D11_SHADER_BUFFER_DESC
{
    char const *Name;
    //D3D_CBUFFER_TYPE Type;
    uint32_t Variables;
    uint32_t Size;
    uint32_t uFlags;
};

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
    struct ID3D11ShaderReflectionConstantBuffer *GetConstantBufferByName(char const *name) { return  nullptr; }

    void AddRef() { ++m_refcount; }
    void Release() { if (--m_refcount <= 0) delete this; }

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
    void Release() { if (--m_refcount <= 0) delete this; }

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
    /* FIXME: implement me */
    return E_FAIL;
}

static inline
HRESULT D3DReflect(void const *pSrcData,
                   size_t SrcDataSize,
                   REFIID pInterface,
                   void **ppReflector)
{
    /* FIXME: implement me */
    return E_FAIL;
}

static inline
HRESULT D3DStripShader(void const *pShaderBytecode,
                       size_t BytecodeLength,
                       uint32_t uStripFlags,
                       ID3DBlob **ppStrippedBlob)
{
    /* FIXME: implement me */
    return E_FAIL;
}

typedef decltype(&d3d4linux::compile) pD3DCompile;

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
    if (!strcmp(name, "D3DDisassemble"))
        return nullptr; /* FIXME */
    if (!strcmp(name, "D3DStripShader"))
        return nullptr; /* FIXME */
    if (!strcmp(name, "D3DCreateBlob"))
        return (void *)&d3d4linux::create_blob;
    return nullptr;
}

