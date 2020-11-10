#pragma once
#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

// Resources:

struct EZ_RHI_DLL GraphicsDeviceChild
{
  std::shared_ptr<void> internal_state;
  inline bool IsValid() const { return internal_state.get() != nullptr; }
};

struct EZ_RHI_DLL Shader : public GraphicsDeviceChild
{
  ezEnum<ezRHIShaderStage> stage = ezRHIShaderStage::ENUM_COUNT;
  std::vector<ezUInt8> code;
  const RootSignature* rootSignature = nullptr;
};

struct EZ_RHI_DLL Sampler : public GraphicsDeviceChild
{
  SamplerDesc desc;

  const SamplerDesc& GetDesc() const { return desc; }
};

struct EZ_RHI_DLL GPUResource : public GraphicsDeviceChild
{
  enum class GPU_RESOURCE_TYPE
  {
    BUFFER,
    TEXTURE,
    RAYTRACING_ACCELERATION_STRUCTURE,
    UNKNOWN_TYPE,
  } type = GPU_RESOURCE_TYPE::UNKNOWN_TYPE;
  inline bool IsTexture() const { return type == GPU_RESOURCE_TYPE::TEXTURE; }
  inline bool IsBuffer() const { return type == GPU_RESOURCE_TYPE::BUFFER; }
  inline bool IsAccelerationStructure() const { return type == GPU_RESOURCE_TYPE::RAYTRACING_ACCELERATION_STRUCTURE; }
};

struct EZ_RHI_DLL GPUBuffer : public GPUResource
{
  GPUBufferDesc desc;

  const GPUBufferDesc& GetDesc() const { return desc; }
};

struct EZ_RHI_DLL InputLayout : public GraphicsDeviceChild
{
  std::vector<InputLayoutDesc> desc;
};

struct EZ_RHI_DLL BlendState : public GraphicsDeviceChild
{
  BlendStateDesc desc;

  const BlendStateDesc& GetDesc() const { return desc; }
};

struct EZ_RHI_DLL DepthStencilState : public GraphicsDeviceChild
{
  DepthStencilStateDesc desc;

  const DepthStencilStateDesc& GetDesc() const { return desc; }
};

struct EZ_RHI_DLL RasterizerState : public GraphicsDeviceChild
{
  RasterizerStateDesc desc;

  const RasterizerStateDesc& GetDesc() const { return desc; }
};

struct EZ_RHI_DLL Texture : public GPUResource
{
  TextureDesc desc;

  const TextureDesc& GetDesc() const { return desc; }
};

struct EZ_RHI_DLL GPUQuery : public GraphicsDeviceChild
{
  GPUQueryDesc desc;

  const GPUQueryDesc& GetDesc() const { return desc; }
};

struct EZ_RHI_DLL PipelineState : public GraphicsDeviceChild
{
  size_t hash = 0;
  PipelineStateDesc desc;

  const PipelineStateDesc& GetDesc() const { return desc; }
};

struct EZ_RHI_DLL RenderPass : public GraphicsDeviceChild
{
  size_t hash = 0;
  RenderPassDesc desc;

  const RenderPassDesc& GetDesc() const { return desc; }
};


struct EZ_RHI_DLL RaytracingAccelerationStructureDesc
{
  enum FLAGS
  {
    FLAG_EMPTY = 0,
    FLAG_ALLOW_UPDATE = 1 << 0,
    FLAG_ALLOW_COMPACTION = 1 << 1,
    FLAG_PREFER_FAST_TRACE = 1 << 2,
    FLAG_PREFER_FAST_BUILD = 1 << 3,
    FLAG_MINIMIZE_MEMORY = 1 << 4,
  };
  ezUInt32 _flags = FLAG_EMPTY;

  enum TYPE
  {
    BOTTOMLEVEL,
    TOPLEVEL,
  } type = BOTTOMLEVEL;

  struct BottomLevel
  {
    struct Geometry
    {
      enum FLAGS
      {
        FLAG_EMPTY = 0,
        FLAG_OPAQUE = 1 << 0,
        FLAG_NO_DUPLICATE_ANYHIT_INVOCATION = 1 << 1,
        FLAG_USE_TRANSFORM = 1 << 2,
      };
      ezUInt32 _flags = FLAG_EMPTY;

      enum TYPE
      {
        TRIANGLES,
        PROCEDURAL_AABBS,
      } type = TRIANGLES;

      struct Triangles
      {
        GPUBuffer vertexBuffer;
        GPUBuffer indexBuffer;
        ezUInt32 indexCount = 0;
        ezUInt32 indexOffset = 0;
        ezUInt32 vertexCount = 0;
        ezUInt32 vertexByteOffset = 0;
        ezUInt32 vertexStride = 0;
        ezRHIIndexBufferFormat::Enum indexFormat = ezRHIIndexBufferFormat::UInt32;
        ezRHIFormat::Enum vertexFormat = ezRHIFormat::R32G32B32_FLOAT;
        GPUBuffer transform3x4Buffer;
        ezUInt32 transform3x4BufferOffset = 0;
      } triangles;
      struct Procedural_AABBs
      {
        GPUBuffer aabbBuffer;
        ezUInt32 offset = 0;
        ezUInt32 count = 0;
        ezUInt32 stride = 0;
      } aabbs;
    };
    std::vector<Geometry> geometries;
  } bottomlevel;

  struct TopLevel
  {
    struct Instance
    {
      Float3X4 transform;
      ezUInt32 InstanceID : 24;
      ezUInt32 InstanceMask : 8;
      ezUInt32 InstanceContributionToHitGroupIndex : 24;
      ezUInt32 Flags : 8;
      GPUResource bottomlevel;
    };
    GPUBuffer instanceBuffer;
    ezUInt32 offset = 0;
    ezUInt32 count = 0;
  } toplevel;
};
struct EZ_RHI_DLL RaytracingAccelerationStructure : public GPUResource
{
  RaytracingAccelerationStructureDesc desc;

  const RaytracingAccelerationStructureDesc& GetDesc() const { return desc; }
};

struct EZ_RHI_DLL ShaderLibrary
{
  enum TYPE
  {
    RAYGENERATION,
    MISS,
    CLOSESTHIT,
    ANYHIT,
    INTERSECTION,
  } type = RAYGENERATION;
  const Shader* shader = nullptr;
  ezString function_name;
};
struct EZ_RHI_DLL ShaderHitGroup
{
  enum TYPE
  {
    GENERAL, // raygen or miss
    TRIANGLES,
    PROCEDURAL,
  } type = TRIANGLES;
  ezString name;
  ezUInt32 general_shader = ~0;
  ezUInt32 closesthit_shader = ~0;
  ezUInt32 anyhit_shader = ~0;
  ezUInt32 intersection_shader = ~0;
};
struct EZ_RHI_DLL RaytracingPipelineStateDesc
{
  const RootSignature* rootSignature = nullptr;
  std::vector<ShaderLibrary> shaderlibraries;
  std::vector<ShaderHitGroup> hitgroups;
  ezUInt32 max_trace_recursion_depth = 1;
  ezUInt32 max_attribute_size_in_bytes = 0;
  ezUInt32 max_payload_size_in_bytes = 0;
};
struct EZ_RHI_DLL RaytracingPipelineState : public GraphicsDeviceChild
{
  RaytracingPipelineStateDesc desc;

  const RaytracingPipelineStateDesc& GetDesc() const { return desc; }
};

struct EZ_RHI_DLL ShaderTable
{
  const GPUBuffer* buffer = nullptr;
  ezUInt64 offset = 0;
  ezUInt64 size = 0;
  ezUInt64 stride = 0;
};
struct EZ_RHI_DLL DispatchRaysDesc
{
  ShaderTable raygeneration;
  ShaderTable miss;
  ShaderTable hitgroup;
  ShaderTable callable;
  ezUInt32 Width = 1;
  ezUInt32 Height = 1;
  ezUInt32 Depth = 1;
};

enum BINDPOINT
{
  GRAPHICS,
  COMPUTE,
  RAYTRACING,
};
enum RESOURCEBINDING
{
  ROOT_CONSTANTBUFFER,
  ROOT_RAWBUFFER,
  ROOT_STRUCTUREDBUFFER,
  ROOT_RWRAWBUFFER,
  ROOT_RWSTRUCTUREDBUFFER,

  CONSTANTBUFFER,
  RAWBUFFER,
  STRUCTUREDBUFFER,
  TYPEDBUFFER,
  TEXTURE1D,
  TEXTURE1DARRAY,
  TEXTURE2D,
  TEXTURE2DARRAY,
  TEXTURECUBE,
  TEXTURECUBEARRAY,
  TEXTURE3D,
  ACCELERATIONSTRUCTURE,
  RWRAWBUFFER,
  RWSTRUCTUREDBUFFER,
  RWTYPEDBUFFER,
  RWTEXTURE1D,
  RWTEXTURE1DARRAY,
  RWTEXTURE2D,
  RWTEXTURE2DARRAY,
  RWTEXTURE3D,

  RESOURCEBINDING_COUNT
};
struct EZ_RHI_DLL ResourceRange
{
  RESOURCEBINDING binding = CONSTANTBUFFER;
  ezUInt32 slot = 0;
  ezUInt32 count = 1;
};
struct EZ_RHI_DLL SamplerRange
{
  ezUInt32 slot = 0;
  ezUInt32 count = 1;
};
struct EZ_RHI_DLL StaticSampler
{
  Sampler sampler;
  ezUInt32 slot = 0;
};
struct EZ_RHI_DLL DescriptorTable : public GraphicsDeviceChild
{
  ezEnum<ezRHIShaderStage> stage = ezRHIShaderStage::ENUM_COUNT;
  std::vector<ResourceRange> resources;
  std::vector<SamplerRange> samplers;
  std::vector<StaticSampler> staticsamplers;
};
struct EZ_RHI_DLL RootConstantRange
{
  ezEnum<ezRHIShaderStage> stage = ezRHIShaderStage::ENUM_COUNT;
  ezUInt32 slot = 0;
  ezUInt32 size = 0;
  ezUInt32 offset = 0;
};
struct EZ_RHI_DLL RootSignature : public GraphicsDeviceChild
{
  enum FLAGS
  {
    FLAG_EMPTY = 0,
    FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT = 1 << 0,
  };
  ezUInt32 _flags = FLAG_EMPTY;
  std::vector<DescriptorTable> tables;
  std::vector<RootConstantRange> rootconstants;
};
