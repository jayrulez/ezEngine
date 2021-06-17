#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Graphics.h>

/*
typedef ezInt32 CommandList;
static const CommandList COMMANDLIST_COUNT = 32;
static const CommandList INVALID_COMMANDLIST = COMMANDLIST_COUNT;
*/

enum QUEUE_TYPE
{
  QUEUE_GRAPHICS,
  QUEUE_COMPUTE,

  QUEUE_COUNT,
};

class ezRHICommandList;

class EZ_RHI_DLL ezRHIGraphicsDevice
{
protected:
  static const uint32_t BUFFERCOUNT = 2;
  uint64_t FRAMECOUNT = 0;
  bool DEBUGDEVICE = false;
  uint32_t capabilities = 0;
  size_t SHADER_IDENTIFIER_SIZE = 0;
  size_t TOPLEVEL_ACCELERATION_STRUCTURE_INSTANCE_SIZE = 0;
  uint32_t VARIABLE_RATE_SHADING_TILE_SIZE = 0;
  uint64_t TIMESTAMP_FREQUENCY = 0;

public:
  virtual ~ezRHIGraphicsDevice() = default;

  virtual bool CreateSwapChain(const SwapChainDesc* pDesc, SwapChain* swapChain) const = 0;
  virtual bool CreateBuffer(const GPUBufferDesc* pDesc, const SubresourceData* pInitialData, GPUBuffer* pBuffer) const = 0;
  virtual bool CreateTexture(const TextureDesc* pDesc, const SubresourceData* pInitialData, Texture* pTexture) const = 0;
  virtual bool CreateShader(SHADERSTAGE stage, const void* pShaderBytecode, ezUInt64 BytecodeLength, Shader* pShader) const = 0;
  virtual bool CreateSampler(const SamplerDesc* pSamplerDesc, Sampler* pSamplerState) const = 0;
  virtual bool CreateQueryHeap(const GPUQueryHeapDesc* pDesc, GPUQueryHeap* pQueryHeap) const = 0;
  virtual bool CreatePipelineState(const PipelineStateDesc* pDesc, PipelineState* pso) const = 0;
  virtual bool CreateRenderPass(const RenderPassDesc* pDesc, RenderPass* renderpass) const = 0;

  virtual int CreateSubresource(Texture* texture, SUBRESOURCE_TYPE type, uint32_t firstSlice, uint32_t sliceCount, uint32_t firstMip, uint32_t mipCount) const = 0;
  virtual int CreateSubresource(GPUBuffer* buffer, SUBRESOURCE_TYPE type, uint64_t offset, uint64_t size = ~0) const = 0;

  virtual int GetDescriptorIndex(const GPUResource* resource, SUBRESOURCE_TYPE type, int subresource = -1) const { return -1; };
  virtual int GetDescriptorIndex(const Sampler* sampler) const { return -1; };

  virtual void WriteShadingRateValue(SHADING_RATE rate, void* dest) const {};

  virtual void Map(const GPUResource* resource, Mapping* mapping) const = 0;
  virtual void Unmap(const GPUResource* resource) const = 0;
  virtual void QueryRead(const GPUQueryHeap* heap, uint32_t index, uint32_t count, uint64_t* results) const = 0;

  virtual void SetCommonSampler(const StaticSampler* sam) = 0;

  virtual void SetName(GPUResource* pResource, const char* name) = 0;

  // Begin a new command list for GPU command recording.
  //	This will be valid until SubmitCommandLists() is called.
  virtual ezRHICommandList* BeginCommandList(QUEUE_TYPE queue = QUEUE_GRAPHICS) = 0;
  // Submit all command list that were used with BeginCommandList before this call.
  //	This will make every command list to be in "available" state and restarts them
  virtual void SubmitCommandLists() = 0;

  virtual void WaitForGPU() const = 0;
  virtual void ClearPipelineStateCache(){};

  constexpr uint64_t GetFrameCount() const { return FRAMECOUNT; }

  inline bool CheckCapability(GRAPHICSDEVICE_CAPABILITY capability) const { return capabilities & capability; }

  uint32_t GetFormatStride(FORMAT value) const;
  bool IsFormatUnorm(FORMAT value) const;
  bool IsFormatBlockCompressed(FORMAT value) const;
  bool IsFormatStencilSupport(FORMAT value) const;

  static constexpr uint32_t GetBufferCount() { return BUFFERCOUNT; }

  constexpr bool IsDebugDevice() const { return DEBUGDEVICE; }

  constexpr size_t GetShaderIdentifierSize() const { return SHADER_IDENTIFIER_SIZE; }
  constexpr size_t GetTopLevelAccelerationStructureInstanceSize() const { return TOPLEVEL_ACCELERATION_STRUCTURE_INSTANCE_SIZE; }
  constexpr uint32_t GetVariableRateShadingTileSize() const { return VARIABLE_RATE_SHADING_TILE_SIZE; }
  constexpr uint64_t GetTimestampFrequency() const { return TIMESTAMP_FREQUENCY; }

  virtual SHADERFORMAT GetShaderFormat() const { return SHADERFORMAT_NONE; }

  virtual Texture GetBackBuffer(const SwapChain* swapchain) const = 0;
};

#include <RHI/Device/Implementation/GraphicsDevice_inl.h>
