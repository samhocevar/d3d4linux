#pragma once

#include <cstdint> /* for uint32_t */
#include <cstddef> /* for size_t */
#include <cstdio> /* for FILE */
#include <cstring> /* for strcmp() */

#include <unistd.h> /* for fork() */
#include <sys/wait.h> /* for waitpid() */
#include <fcntl.h> /* for O_WRONLY */

#include <string> /* for std::string */

struct d3d4linux
{
    static HRESULT create_blob(size_t Size,
                               ID3DBlob **ppBlob)
    {
        *ppBlob = new ID3DBlob(Size);
        return 0;
    }

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
        FILE *in, *out;
        int pipe_read[2], pipe_write[2];
        HRESULT ret = -1;

        pipe(pipe_read);
        pipe(pipe_write);

        pid_t pid = fork();
        switch (pid)
        {
        case -1:
            static char const *error_msg = "Error forking d3d4linux-server";
            create_blob(strlen(error_msg), ppErrorMsgs);
            memcpy((*ppErrorMsgs)->GetBufferPointer(), error_msg, (*ppErrorMsgs)->GetBufferSize());
            return D3D10_ERROR_FILE_NOT_FOUND;

        case 0:
            dup2(pipe_write[0], STDIN_FILENO);
            dup2(pipe_read[1], STDOUT_FILENO);
            dup2(open("/dev/null", O_WRONLY), STDERR_FILENO);

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

            fprintf(out, "s%s%c", (char const *)pSrcData, '\0');
            if (pFileName)
                fprintf(out, "f%s%c", pFileName, '\0');
            fprintf(out, "m%s%c" "t%s%c" "1%d%c" "2%d%c" "X",
                    pEntrypoint, '\0', pTarget, '\0', Flags1, '\0', Flags2, '\0');
            fflush(out);

            for (bool running = true; running; )
            {
                int ch = getc(in);
                if (ch < 0)
                    break;

                switch (ch)
                {
                case 'r':
                    ret = read_long(in);
                    break;
                case 'l':
                    *ppCode = read_blob(in);
                    break;
                case 'e':
                    *ppErrorMsgs = read_blob(in);
                    break;
                case 'q':
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

private:
    static long read_long(FILE *in)
    {
        std::string tmp;
        int ch;
        while ((ch = getc(in)) > 0)
            tmp += ch;
        return atol(tmp.c_str());
    }

    static ID3DBlob *read_blob(FILE *in)
    {
        ID3DBlob *ret;
        std::string tmp;
        int ch;

        while ((ch = getc(in)) > 0)
            tmp += ch;

        create_blob(atoll(tmp.c_str()), &ret);
        fread(ret->GetBufferPointer(), ret->GetBufferSize(), 1, in);
        return ret;
    }
};

