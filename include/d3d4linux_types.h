#pragma once

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
    D3D_RESOURCE_RETURN_TYPE ReturnType;
    D3D_SRV_DIMENSION Dimension;
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
    D3D_PRIMITIVE_TOPOLOGY GSOutputTopology;
    uint32_t GSMaxOutputVertexCount;
    D3D_PRIMITIVE InputPrimitive;
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
    D3D_CBUFFER_TYPE Type;
    uint32_t Variables;
    uint32_t Size;
    uint32_t uFlags;
};

