
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
    v->resize(len);
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
    HMODULE lib = LoadLibrary("d3dcompiler_43.dll");

    /* Ensure stdout is in binary mode */
    setmode(fileno(stdout), O_BINARY);
    setmode(fileno(stdin), O_BINARY);

    int syscall = read_integer();
    int marker = 0;

    if (syscall == D3D4LINUX_COMPILE)
    {
        HRESULT (*compile)(void const *pSrcData, size_t SrcDataSize,
                           char const *pFileName,
                           D3D_SHADER_MACRO const *pDefines,
                           ID3DInclude *pInclude,
                           char const *pEntrypoint, char const *pTarget,
                           uint32_t Flags1, uint32_t Flags2,
                           ID3DBlob **ppCode, ID3DBlob **ppErrorMsgs);
        compile = (decltype(compile))GetProcAddress(lib, "D3DCompile");

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
        HRESULT (*reflect)(void const *pSrcData,
                           size_t SrcDataSize,
                           REFIID pInterface,
                           void **ppReflector);
        reflect = (decltype(reflect))GetProcAddress(lib, "D3DReflect");

        std::vector<uint8_t> *data = read_data();
        int iid_code = read_integer();
        marker = (int)read_integer();
        if (marker != D3D4LINUX_FINISHED)
            goto error;

        char const *iid_name = "";
        IID iid;
        switch (iid_code)
        {
        case D3D4LINUX_IID_SHADER_REFLECTION:
            iid = IID_ID3D11ShaderReflection;
            iid_name = "IID_ID3D11ShaderReflection";
            break;
        default:
            goto error;
        }

        fprintf(stderr, "[SERVER] D3DReflect([%d bytes], %s <unfinished ...>\n",
                data ? (int)data->size() : 0, iid_name);
        void *reflector;
        HRESULT ret = reflect(data ? data->data() : nullptr,
                              data ? data->size() : 0,
                              iid, &reflector);
        fprintf(stderr, "[SERVER] < ... D3DReflect resumed> ) = 0x%08x\n", (int)ret);

        write_integer(ret);
        write_integer(D3D4LINUX_FINISHED);

        delete data;
    }
    else if (syscall == D3D4LINUX_STRIP)
    {
        HRESULT (*strip)(void const *pShaderBytecode,
                         size_t BytecodeLength,
                         uint32_t uStripFlags,
                         ID3DBlob **ppStrippedBlob);
        strip = (decltype(strip))GetProcAddress(lib, "D3DStripShader");

        std::vector<uint8_t> *data = read_data();
        uint32_t flags = (uint32_t)read_integer();
        marker = (int)read_integer();
        if (marker != D3D4LINUX_FINISHED)
            goto error;

        fprintf(stderr, "[SERVER] D3DStripShader([%d bytes], %04x <unfinished ...>\n",
                data ? (int)data->size() : 0, flags);
        ID3DBlob *strip_blob = nullptr;
        HRESULT ret = strip(data ? data->data() : nullptr,
                            data ? data->size() : 0,
                            flags, &strip_blob);
        fprintf(stderr, "[SERVER] < ... D3DStripShader resumed> ) = 0x%08x\n", (int)ret);

        write_integer(ret);
        write_blob(strip_blob);
        write_integer(D3D4LINUX_FINISHED);

        if (strip_blob)
            strip_blob->Release();
    }

    return EXIT_SUCCESS;

error:
    fprintf(stderr, "[SERVER] Bad message received: %08x %08x\n", syscall, marker);
}

