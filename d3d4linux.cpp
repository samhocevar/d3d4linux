
#include <d3dcompiler.h>

#include <string>

#include <cstdio>
#include <cstdint>

static std::string read_string()
{
    std::string tmp;
    int ch;
    while ((ch = getchar()) > 0)
        tmp += ch;
    return tmp;
}

static int64_t read_integer()
{
    return atoll(read_string().c_str());
}

static void write_blob(ID3DBlob *blob)
{
    fprintf(stdout, "%d%c", blob ? (int)blob->GetBufferSize() : -1, '\0');
    if (blob)
    {
        fwrite(blob->GetBufferPointer(), (int)blob->GetBufferSize(), 1, stdout);
        blob->Release();
    }
    fflush(stdout);
}

static void write_integer(int64_t x)
{
    fprintf(stdout, "%lld%c", (long long int)x, '\0');
    fflush(stdout);
}

int main(void)
{
    ID3DBlob *shader_blob, *error_blob;

    HMODULE lib = LoadLibrary("d3dcompiler_47.dll");
    pD3DCompile compile = (pD3DCompile)GetProcAddress(lib, "D3DCompile");

    int syscall = read_integer();

    if (syscall == 1)
    {
        /* This is a D3DCompile() call */
        std::string shader_source = read_string();

        int has_filename = (int)read_integer();
        std::string shader_file;
        if (has_filename)
            shader_file = read_string();

        std::string shader_main = read_string();
        std::string shader_type = read_string();
        uint32_t flags1 = (uint32_t)read_integer();
        uint32_t flags2 = (uint32_t)read_integer();

        fprintf(stderr, "[SERVER] D3DCompile([%d bytes], \"%s\", ?, ?, \"%s\", \"%s\", %04x, %04x <unfinished ...>\n",
                (int)shader_source.size(), has_filename ? shader_file.c_str() : "(nullptr)", shader_main.c_str(), shader_type.c_str(),
                flags1, flags2);
        HRESULT ret = compile(shader_source.c_str(), shader_source.size(),
                              shader_file.c_str(),
                              nullptr, /* unimplemented */
                              nullptr, /* unimplemented */
                              shader_main.c_str(),
                              shader_type.c_str(),
                              flags1, flags2, &shader_blob, &error_blob);
        fprintf(stderr, "[SERVER] < ... D3DCompile resumed> ) = 0x%08x\n", (int)ret);

        write_integer(ret);
        write_blob(shader_blob);
        write_blob(error_blob);
    }
}

