#pragma once

#include <cstdint> /* for uint32_t */
#include <cstddef> /* for size_t */
#include <cstdio> /* for FILE */
#include <cstring> /* for strcmp() */

#include <unistd.h> /* for fork() */
#include <sys/wait.h> /* for waitpid() */
#include <fcntl.h> /* for O_WRONLY */

#include <string> /* for std::string */

#include <d3d4linux_common.h>

struct d3d4linux
{
    static HRESULT compile(void const *pSrcData,
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
        fork_process p;
        if (p.error())
        {
            static char const *error_msg = "Cannot fork in d3d4linux::compile()";
            *ppErrorMsgs = new ID3DBlob(strlen(error_msg));
            memcpy((*ppErrorMsgs)->GetBufferPointer(), error_msg, (*ppErrorMsgs)->GetBufferSize());
            return E_FAIL;
        }

        p.write_long(D3D4LINUX_COMPILE);
        p.write_string((char const *)pSrcData);
        p.write_long(pFileName ? 1 : 0);
        p.write_string(pFileName ? pFileName : "");
        p.write_string(pEntrypoint);
        p.write_string(pTarget);
        p.write_long(Flags1);
        p.write_long(Flags2);
        p.write_long(D3D4LINUX_FINISHED);

        HRESULT ret = p.read_long();
        ID3DBlob *code_blob = p.read_blob();
        ID3DBlob *error_blob = p.read_blob();
        int end = p.read_long();
        if (end != D3D4LINUX_FINISHED)
            return E_FAIL;

        *ppCode = code_blob;
        *ppErrorMsgs = error_blob;
        return ret;
    }

    static HRESULT reflect(void const *pSrcData,
                           size_t SrcDataSize,
                           REFIID pInterface,
                           void **ppReflector)
    {
        fork_process p;
        if (p.error())
            return E_FAIL;

        p.write_long(D3D4LINUX_REFLECT);
        p.write_data(pSrcData, SrcDataSize);
        p.write_long(pInterface);
        p.write_long(D3D4LINUX_FINISHED);

        HRESULT ret = p.read_long();
        int end = p.read_long();
        if (end != D3D4LINUX_FINISHED)
            return E_FAIL;

        return ret;
    }

    static HRESULT strip_shader(void const *pShaderBytecode,
                                size_t BytecodeLength,
                                uint32_t uStripFlags,
                                ID3DBlob **ppStrippedBlob)
    {
        /* FIXME: implement me */
        return E_FAIL;
    }

    static HRESULT disassemble(void const *pSrcData,
                               size_t SrcDataSize,
                               uint32_t Flags,
                               char const *szComments,
                               ID3DBlob **ppDisassembly)
    {
        /* FIXME: implement me */
        return E_FAIL;
    }

    static HRESULT create_blob(size_t Size,
                               ID3DBlob **ppBlob)
    {
        *ppBlob = new ID3DBlob(Size);
        return S_OK;
    }

private:
    struct fork_process
    {
    public:
        fork_process()
          : m_in(nullptr),
            m_out(nullptr),
            m_pid(-1)
        {
            pipe(m_pipe_read);
            pipe(m_pipe_write);

            m_pid = fork();

            if (m_pid == 0)
            {
                dup2(m_pipe_write[0], STDIN_FILENO);
                dup2(m_pipe_read[1], STDOUT_FILENO);
                //dup2(open("/dev/null", O_WRONLY), STDERR_FILENO);

                close(m_pipe_read[0]);
                close(m_pipe_read[1]);
                close(m_pipe_write[0]);
                close(m_pipe_write[1]);

                static char *const argv[] = { (char *)"wine", (char *)"/home/sam/d3d4linux/d3d4linux.exe", 0 };
                execv("/usr/bin/wine", argv);
                /* Never going past here */
            }

            close(m_pipe_write[0]);
            close(m_pipe_read[1]);

            if (m_pid < 0)
                return;

            m_in = fdopen(m_pipe_read[0], "r");
            m_out = fdopen(m_pipe_write[1], "w");
        }

        ~fork_process()
        {
            if (m_pid > 0)
            {
                waitpid(m_pid, nullptr, 0);
                fclose(m_in);
                fclose(m_out);
            }

            close(m_pipe_read[0]);
            close(m_pipe_write[1]);
        }

        bool error() const
        {
            return m_pid <= 0;
        }

        void write_long(long x)
        {
            fprintf(m_out, "%ld%c", x, '\0');
            if (x == D3D4LINUX_FINISHED)
                fflush(m_out);
        }

        void write_string(char const *s)
        {
            fprintf(m_out, "%s%c", s, '\0');
        }

        void write_data(void const *data, size_t len)
        {
            fprintf(m_out, "%ld%c", (long)len, '\0');
            fwrite(data, len, 1, m_out);
        }

        void write_blob(ID3DBlob *blob)
        {
            write_long(blob ? blob->GetBufferSize() : -1);
            if (blob)
                fwrite(blob->GetBufferPointer(), blob->GetBufferSize(), 1, stdout);
        }

        long read_long()
        {
            std::string tmp;
            int ch;
            while ((ch = fgetc(m_in)) > 0)
                tmp += ch;
            return atol(tmp.c_str());
        }

        ID3DBlob *read_blob()
        {
            int len = read_long();
            if (len < 0)
                return nullptr;

            ID3DBlob *blob = new ID3DBlob(len);
            fread(blob->GetBufferPointer(), len, 1, m_in);
            return blob;
        }

    private:
        FILE *m_in, *m_out;
        int m_pipe_read[2], m_pipe_write[2];
        pid_t m_pid;
    };
};

