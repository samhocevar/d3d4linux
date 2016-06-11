
#include <string>
#include <vector>

#include <cstdio>
#include <cstdint>
#include <fcntl.h>

#include <d3dcompiler.h>

#include <d3d4linux_common.h>

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

static std::vector<uint8_t> *read_data()
{
    size_t len = (size_t)read_integer();
    if (len < 0)
        return nullptr;
    std::vector<uint8_t> *v = new std::vector<uint8_t>();
    fread(v->data(), (int)v->size(), 1, stdin);
    return v;
}

static void write_integer(int64_t x)
{
    fprintf(stdout, "%lld%c", (long long int)x, '\0');
    if (x == D3D4LINUX_FINISHED)
        fflush(stdout);
}

static void write_blob(ID3DBlob *blob)
{
    int size = blob ? (int)blob->GetBufferSize() : -1;
    write_integer(size);
    if (size > 0)
        fwrite(blob->GetBufferPointer(), size, 1, stdout);
}

int main(void)
{
    HMODULE lib = LoadLibrary("d3dcompiler_47.dll");
    pD3DCompile compile = (pD3DCompile)GetProcAddress(lib, "D3DCompile");

    /* Ensure stdout is in binary mode */
    setmode(fileno(stdout), O_BINARY);

    int syscall = read_integer();
    int marker = 0;

    if (syscall == D3D4LINUX_COMPILE)
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
        marker = (int)read_integer();
        if (marker != D3D4LINUX_FINISHED)
            goto error;

        fprintf(stderr, "[SERVER] D3DCompile([%d bytes], \"%s\", ?, ?, \"%s\", \"%s\", %04x, %04x <unfinished ...>\n",
                (int)shader_source.size(), has_filename ? shader_file.c_str() : "(nullptr)", shader_main.c_str(), shader_type.c_str(),
                flags1, flags2);
        ID3DBlob *shader_blob = nullptr, *error_blob = nullptr;
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
        write_integer(D3D4LINUX_FINISHED);

        if (shader_blob)
            shader_blob->Release();
        if (error_blob)
            error_blob->Release();
    }
    else if (syscall == D3D4LINUX_REFLECT)
    {
        std::vector<uint8_t> *data = read_data();
        int iid = (int)read_integer();
        (void)iid;

        write_integer(E_FAIL);
        write_integer(D3D4LINUX_FINISHED);

        delete data;
    }

    return EXIT_SUCCESS;

error:
    fprintf(stderr, "[SERVER] Bad message received: %08x %08x\n", syscall, marker);
}

