//
//  D3D4Linux — access Direct3D DLLs from Linux programs
//
//  Copyright © 2016 Sam Hocevar <sam@hocevar.net>
//
//  This library is free software. It comes without any warranty, to
//  the extent permitted by applicable law. You can redistribute it
//  and/or modify it under the terms of the Do What the Fuck You Want
//  to Public License, Version 2, as published by the WTFPL Task Force.
//  See http://www.wtfpl.net/ for more details.
//

#include <string>
#include <vector>

#include <cstdio>
#include <cstdint>
#include <fcntl.h>

#include <d3dcompiler.h>

#include <d3d4linux_common.h>

int main(void)
{
    char const *verbose_var = getenv("D3D4LINUX_VERBOSE");
    int verbose = verbose_var && *verbose_var == '1';

    char const *dll_var = getenv("D3D4LINUX_DLL");
    dll_var = dll_var ? dll_var : "d3dcompiler_47.dll";

    HMODULE lib = LoadLibrary(dll_var);

    /* Ensure stdout is in binary mode */
    setmode(fileno(stdout), O_BINARY);
    setmode(fileno(stdin), O_BINARY);

    interop p(stdin, stdout);

    while (!feof(stdin))
    {
        int syscall = p.read_i64();
        int marker = 0;

        if (syscall == D3D4LINUX_OP_COMPILE)
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
            std::string shader_source = p.read_string();

            int has_filename = (int)p.read_i64();
            std::string shader_file;
            if (has_filename)
                shader_file = p.read_string();

            std::string shader_main = p.read_string();
            std::string shader_type = p.read_string();
            uint32_t flags1 = (uint32_t)p.read_i64();
            uint32_t flags2 = (uint32_t)p.read_i64();
            marker = (int)p.read_i64();
            if (marker != D3D4LINUX_FINISHED)
                goto error;

            ID3DBlob *shader_blob = nullptr, *error_blob = nullptr;
            HRESULT ret = compile(shader_source.c_str(), shader_source.size(),
                                  shader_file.c_str(),
                                  nullptr, /* unimplemented */
                                  nullptr, /* unimplemented */
                                  shader_main.c_str(),
                                  shader_type.c_str(),
                                  flags1, flags2, &shader_blob, &error_blob);
            if (verbose)
                fprintf(stderr, "[D3D4LINUX] D3DCompile([%d bytes], \"%s\", ?, ?, \"%s\", \"%s\", %04x, %04x ) = 0x%x\n",
                        (int)shader_source.size(), has_filename ? shader_file.c_str() : "(nullptr)", shader_main.c_str(), shader_type.c_str(),
                        flags1, flags2, (int)ret);

            p.write_i64(ret);
            p.write_blob(shader_blob);
            p.write_blob(error_blob);
            p.write_i64(D3D4LINUX_FINISHED);

            if (shader_blob)
                shader_blob->Release();
            if (error_blob)
                error_blob->Release();
        }
        else if (syscall == D3D4LINUX_OP_REFLECT)
        {
            HRESULT (*reflect)(void const *pSrcData,
                               size_t SrcDataSize,
                               REFIID pInterface,
                               void **ppReflector);
            reflect = (decltype(reflect))GetProcAddress(lib, "D3DReflect");

            std::vector<uint8_t> *data = p.read_data();
            int iid_code = p.read_i64();
            marker = (int)p.read_i64();
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
                fprintf(stderr, "[D3D4LINUX] unknown iid_code %d\n", iid_code);
                goto error;
            }

            void *object;
            HRESULT ret = reflect(data ? data->data() : nullptr,
                                  data ? data->size() : 0,
                                  iid, &object);
            if (verbose)
                fprintf(stderr, "[D3D4LINUX] D3DReflect([%d bytes], %s) = 0x%x\n",
                        data ? (int)data->size() : 0, iid_name, (int)ret);

            p.write_i64(ret);

            if (iid_code == D3D4LINUX_IID_SHADER_REFLECTION)
            {
                D3D11_SIGNATURE_PARAMETER_DESC param_desc;
                D3D11_SHADER_INPUT_BIND_DESC bind_desc;
                D3D11_SHADER_VARIABLE_DESC variable_desc;
                D3D11_SHADER_BUFFER_DESC buffer_desc;
                D3D11_SHADER_DESC shader_desc;

                ID3D11ShaderReflection *reflector = (ID3D11ShaderReflection *)object;

                /* Serialise D3D11_SHADER_DESC */
                reflector->GetDesc(&shader_desc);
                p.write_raw(&shader_desc, sizeof(shader_desc));
                p.write_string(shader_desc.Creator);

                /* Serialize all InputParameterDesc */
                for (uint32_t i = 0; i < shader_desc.InputParameters; ++i)
                {
                    reflector->GetInputParameterDesc(i, &param_desc);
                    p.write_raw(&param_desc, sizeof(param_desc));
                    p.write_string(param_desc.SemanticName);
                }

                /* Serialize all OutParameterDesc */
                for (uint32_t i = 0; i < shader_desc.OutputParameters; ++i)
                {
                    reflector->GetOutputParameterDesc(i, &param_desc);
                    p.write_raw(&param_desc, sizeof(param_desc));
                    p.write_string(param_desc.SemanticName);
                }

                /* Serialize all ResourceBindingDesc */
                for (uint32_t i = 0; i < shader_desc.BoundResources; ++i)
                {
                    reflector->GetResourceBindingDesc(i, &bind_desc);
                    p.write_raw(&bind_desc, sizeof(bind_desc));
                    p.write_string(bind_desc.Name);
                }

                /* Serialize all ConstantBuffer */
                for (uint32_t i = 0; i < shader_desc.ConstantBuffers; ++i)
                {
                    ID3D11ShaderReflectionConstantBuffer *cbuffer
                            = reflector->GetConstantBufferByIndex(i);

                    /* Serialize D3D11_SHADER_BUFFER_DESC */
                    cbuffer->GetDesc(&buffer_desc);
                    p.write_raw(&buffer_desc, sizeof(buffer_desc));
                    p.write_string(buffer_desc.Name);

                    /* Serialize all Variable */
                    for (uint32_t j = 0; j < buffer_desc.Variables; ++j)
                    {
                        ID3D11ShaderReflectionVariable *var
                                = cbuffer->GetVariableByIndex(j);

                        /* Serialize D3D11_SHADER_VARIABLE_DESC */
                        var->GetDesc(&variable_desc);
                        p.write_raw(&variable_desc, sizeof(variable_desc));
                        p.write_string(variable_desc.Name);
                        p.write_i64(variable_desc.DefaultValue ? 1 : 0);
                        if (variable_desc.DefaultValue)
                            p.write_raw(variable_desc.DefaultValue, variable_desc.Size);
                    }
                }
            }

            p.write_i64(D3D4LINUX_FINISHED);

            delete data;
        }
        else if (syscall == D3D4LINUX_OP_STRIP)
        {
            HRESULT (*strip)(void const *pShaderBytecode,
                             size_t BytecodeLength,
                             uint32_t uStripFlags,
                             ID3DBlob **ppStrippedBlob);
            strip = (decltype(strip))GetProcAddress(lib, "D3DStripShader");

            std::vector<uint8_t> *data = p.read_data();
            uint32_t flags = (uint32_t)p.read_i64();
            marker = (int)p.read_i64();
            if (marker != D3D4LINUX_FINISHED)
                goto error;

            ID3DBlob *strip_blob = nullptr;
            HRESULT ret = strip(data ? data->data() : nullptr,
                                data ? data->size() : 0,
                                flags, &strip_blob);
            if (verbose)
                fprintf(stderr, "[D3D4LINUX] D3DStripShader([%d bytes], %04x) = 0x%x\n",
                        data ? (int)data->size() : 0, flags, (int)ret);

            p.write_i64(ret);
            p.write_blob(strip_blob);
            p.write_i64(D3D4LINUX_FINISHED);

            if (strip_blob)
                strip_blob->Release();
        }
        else if (syscall == D3D4LINUX_OP_DISASSEMBLE)
        {
            HRESULT (*disas)(void const *pSrcData,
                             size_t SrcDataSize,
                             uint32_t Flags,
                             char const *szComments,
                             ID3DBlob **ppDisassembly);
            disas = (decltype(disas))GetProcAddress(lib, "D3DDisassemble");

            std::vector<uint8_t> *data = p.read_data();
            uint32_t flags = (uint32_t)p.read_i64();
            int has_comments = (int)p.read_i64();
            std::string comments;
            if (has_comments)
                comments = p.read_string();
            marker = (int)p.read_i64();
            if (marker != D3D4LINUX_FINISHED)
                goto error;

            ID3DBlob *disas_blob = nullptr;
            HRESULT ret = disas(data ? data->data() : nullptr,
                                data ? data->size() : 0,
                                flags,
                                has_comments ? comments.c_str() : nullptr,
                                &disas_blob);
            if (verbose)
                fprintf(stderr, "[D3D4LINUX] D3DDisassemble([%d bytes], %04x, %s) = 0x%x\n",
                        data ? (int)data->size() : 0, flags, has_comments ? "[comments]" : "(nullptr)", (int)ret);

            p.write_i64(ret);
            p.write_blob(disas_blob);
            p.write_i64(D3D4LINUX_FINISHED);

            if (disas_blob)
                disas_blob->Release();
        }

        continue;

    error:
        if (verbose)
            fprintf(stderr, "[D3D4LINUX] Bad message received: 0x%x 0x%x\n", syscall, marker);
    }

    return EXIT_SUCCESS;
}

