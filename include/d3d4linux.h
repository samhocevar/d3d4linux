
#include <cstdint> /* for uint32_t */
#include <cstddef> /* for size_t */
#include <cstdio> /* for FILE */

#include <string> /* for std::string */

/*
 * Types and macros that come from Windows
 */

typedef int HRESULT;
typedef int HMODULE;

#define FAILED(x) ((x) != 0)

/*
 * Types and macros that mimic some of the ones in D3D
 */

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

struct D3D_SHADER_MACRO
{
    char const *Name;
    char const *Definition;
};

// FIXME
struct ID3DInclude
{
};

struct ID3DBlob
{
    char const *GetBufferPointer() const
    {
        return contents.c_str();
    }

    size_t GetBufferSize() const
    {
        return contents.size();
    }

    void Release()
    {
        delete this;
    }

    std::string contents;
};

typedef long (*pD3DCompile)(void const *pSrcData,
                            size_t SrcDataSize,
                            char const *pFileName,
                            D3D_SHADER_MACRO const *pDefines,
                            ID3DInclude *pInclude,
                            char const *pEntrypoint,
                            char const *pTarget,
                            uint32_t Flags1,
                            uint32_t Flags2,
                            ID3DBlob **ppCode,
                            ID3DBlob **ppErrorMsgs);

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
        {
            fprintf(stderr, "Error launching d3d4linux-server\n");
            return -1;
        }

        fprintf(p, "s%s%c", pSrcData, '\0');
        if (pFileName)
            fprintf(p, "f%s%c", pFileName, '\0');
        fprintf(p, "m%s%c", pEntrypoint, '\0');
        fprintf(p, "t%s%c", pTarget, '\0');
        fprintf(p, "1%d%c", Flags1, '\0');
        fprintf(p, "2%d%c", Flags2, '\0');
        fprintf(p, "X");
        fflush(p);

        pclose(p);
        return -1;
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

/*
 * Helper functions for Windows
 */

static inline HMODULE LoadLibrary(char const *name)
{
    return 0;
}

static void *GetProcAddress(HMODULE, char const *name)
{
    return (void *)&d3d4linux::compile;
}

