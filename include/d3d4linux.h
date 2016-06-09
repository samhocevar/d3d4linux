
#include <cstdint> /* for uint32_t */
#include <cstddef> /* for size_t */
#include <cstdio> /* for FILE */

#include <unistd.h> /* for fork() */
#include <sys/wait.h> /* for waitpid() */

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

struct ID3DInclude
{
    // FIXME
};

struct ID3DBlob
{
    char const *GetBufferPointer() const
    {
        return m_contents.c_str();
    }

    size_t GetBufferSize() const
    {
        return m_contents.size();
    }

    void Release()
    {
        delete this;
    }

    std::string m_contents;
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
        FILE *in, *out;
        int pipe_read[2], pipe_write[2];
        long ret = -1;

        pipe(pipe_read);
        pipe(pipe_write);

        pid_t pid = fork();
        switch (pid)
        {
        case -1:
            fprintf(stderr, "Error forking d3d4linux-server\n");
            return ret;

        case 0:
            dup2(pipe_write[0], STDIN_FILENO);
            dup2(pipe_read[1], STDOUT_FILENO);

            //close(STDERR_FILENO);

            close(pipe_read[0]);
            close(pipe_read[1]);
            close(pipe_write[0]);
            close(pipe_write[1]);

            static char *const argv[] = { (char *)"wine", (char *)"d3d4linux-server.exe", 0 };
            execv("/usr/bin/wine", argv);
            break;

        default:
            close(pipe_write[0]);
            close(pipe_read[1]);

            in = fdopen(pipe_read[0], "r");
            out = fdopen(pipe_write[1], "w");

            fprintf(out, "s%s%c", pSrcData, '\0');
            if (pFileName)
                fprintf(out, "f%s%c", pFileName, '\0');
            fprintf(out, "m%s%c", pEntrypoint, '\0');
            fprintf(out, "t%s%c", pTarget, '\0');
            fprintf(out, "1%d%c", Flags1, '\0');
            fprintf(out, "2%d%c", Flags2, '\0');
            fprintf(out, "X\0");
            fflush(out);

            for (bool running = true; running; )
            {
                std::string tmp;
                int ch = getc(in);
                if (ch < 0)
                    break;

                switch (ch)
                {
                case 'r':
                    while ((ch = getc(in)) > 0)
                        tmp += ch;
                    ret = atoi(tmp.c_str());
                    //fprintf(stderr, "[CLIENT] Got ret = %d\n", ret);
                    if (ret != 0)
                        running = false;
                    break;

                case 'l':
                    *ppCode = new ID3DBlob;

                    while ((ch = getc(in)) > 0)
                        tmp += ch;
                    for (int i = atoi(tmp.c_str()); i--; )
                        (*ppCode)->m_contents += getc(in);
                    //fprintf(stderr, "[CLIENT] Got %d bytes of bytecode\n", (*ppCode)->GetBufferSize());
                    running = false;
                    break;
                }
            }

            fprintf(out, "q");
            fflush(out);
        }

        waitpid(pid, nullptr, 0);

        return ret;
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

