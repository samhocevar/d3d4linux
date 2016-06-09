
#include <cstdint>
#include <cstddef>

/*
 * Types and macros that mimic some of the ones in D3D
 */

#define D3D10_SHADER_DEBUG                          0x0001
#define D3D10_SHADER_SKIP_VALIDATION                0x0002
#define D3D10_SHADER_SKIP_OPTIMIZATION              0x0004
#define D3D10_SHADER_PACK_MATRIX_ROW_MAJOR          0x0008
#define D3D10_SHADER_AVOID_FLOW_CONTROL             0x0200
#define D3D10_SHADER_PREFER_FLOW_CONTROL            0x0400
#define D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY 0x1000

#define D3D10_SHADER_OPTIMIZATION_LEVEL1 0x0000
#define D3D10_SHADER_OPTIMIZATION_LEVEL2 0xc000
#define D3D10_SHADER_OPTIMIZATION_LEVEL3 0x8000

struct D3D_SHADER_MACRO
{
    char const *Name;
    char const *Definition;
};

// FIXME
typedef void ID3DInclude;
typedef void ID3DBlob;

/*
 * Helper struct for compilation
 */

struct d3d4linux
{
    static long compile(void const *pSrcData,
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
        FILE *p = popen("wine d3d4linux-server.exe", "w");
        if (!p)
            return -1;

        pclose(p);
        return 0;
    }
};

static inline
long D3DCompile(void const *pSrcData,
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

