#pragma once
#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <Foundation/Utilities/GraphicsUtils.h>
#include <RHI/Graphics.h>
#include <memory>


typedef ezUInt8 CommandList;
static const CommandList COMMANDLIST_COUNT = 32;

class EZ_RHI_DLL GraphicsDevice
{
protected:
  ezUInt64 FRAMECOUNT = 0;
  bool VSYNC = true;
  int RESOLUTIONWIDTH = 0;
  int RESOLUTIONHEIGHT = 0;
  bool DEBUGDEVICE = false;
  bool FULLSCREEN = false;
  ezRHIFormat::Enum BACKBUFFER_FORMAT = ezRHIFormat::R10G10B10A2_UNORM;
  static const ezUInt32 BACKBUFFER_COUNT = 2;
  bool TESSELLATION = false;
  bool CONSERVATIVE_RASTERIZATION = false;
  bool RASTERIZER_ORDERED_VIEWS = false;
  bool UAV_LOAD_FORMAT_COMMON = false;
  bool UAV_LOAD_FORMAT_R11G11B10_FLOAT = false;
  bool RENDERTARGET_AND_VIEWPORT_ARRAYINDEX_WITHOUT_GS = false;
  bool RAYTRACING = false;
  bool RAYTRACING_INLINE = false;
  bool DESCRIPTOR_MANAGEMENT = false;
  bool VARIABLE_RATE_SHADING = false;
  bool VARIABLE_RATE_SHADING_TIER2 = false;
  bool MESH_SHADER = false;
  size_t SHADER_IDENTIFIER_SIZE = 0;
  size_t TOPLEVEL_ACCELERATION_STRUCTURE_INSTANCE_SIZE = 0;
  ezUInt32 VARIABLE_RATE_SHADING_TILE_SIZE = 0;

public:
  virtual bool CreateBuffer(const GPUBufferDesc* pDesc, const SubresourceData* pInitialData, GPUBuffer* pBuffer) = 0;
  virtual bool CreateTexture(const TextureDesc* pDesc, const SubresourceData* pInitialData, Texture* pTexture) = 0;
  virtual bool CreateInputLayout(const InputLayoutDesc* pInputElementDescs, ezUInt32 NumElements, const Shader* shader, InputLayout* pInputLayout) = 0;
  virtual bool CreateShader(ezEnum<ezRHIShaderStage> stage, const void* pShaderBytecode, size_t BytecodeLength, Shader* pShader) = 0;
  virtual bool CreateBlendState(const BlendStateDesc* pBlendStateDesc, BlendState* pBlendState) = 0;
  virtual bool CreateDepthStencilState(const DepthStencilStateDesc* pDepthStencilStateDesc, DepthStencilState* pDepthStencilState) = 0;
  virtual bool CreateRasterizerState(const RasterizerStateDesc* pRasterizerStateDesc, RasterizerState* pRasterizerState) = 0;
  virtual bool CreateSampler(const SamplerDesc* pSamplerDesc, Sampler* pSamplerState) = 0;
  virtual bool CreateQuery(const GPUQueryDesc* pDesc, GPUQuery* pQuery) = 0;
  virtual bool CreatePipelineState(const PipelineStateDesc* pDesc, PipelineState* pso) = 0;
  virtual bool CreateRenderPass(const RenderPassDesc* pDesc, RenderPass* renderpass) = 0;
  virtual bool CreateRaytracingAccelerationStructure(const RaytracingAccelerationStructureDesc* pDesc, RaytracingAccelerationStructure* bvh) { return false; }
  virtual bool CreateRaytracingPipelineState(const RaytracingPipelineStateDesc* pDesc, RaytracingPipelineState* rtpso) { return false; }
  virtual bool CreateDescriptorTable(DescriptorTable* table) { return false; }
  virtual bool CreateRootSignature(RootSignature* rootsig) { return false; }

  virtual int CreateSubresource(Texture* texture, SUBRESOURCE_TYPE type, ezUInt32 firstSlice, ezUInt32 sliceCount, ezUInt32 firstMip, ezUInt32 mipCount) = 0;
  virtual int CreateSubresource(GPUBuffer* buffer, SUBRESOURCE_TYPE type, ezUInt64 offset, ezUInt64 size = ~0) = 0;

  virtual void WriteShadingRateValue(SHADING_RATE rate, void* dest){};
  virtual void WriteTopLevelAccelerationStructureInstance(const RaytracingAccelerationStructureDesc::TopLevel::Instance* instance, void* dest) {}
  virtual void WriteShaderIdentifier(const RaytracingPipelineState* rtpso, ezUInt32 group_index, void* dest) {}
  virtual void WriteDescriptor(const DescriptorTable* table, ezUInt32 rangeIndex, ezUInt32 arrayIndex, const GPUResource* resource, int subresource = -1, ezUInt64 offset = 0) {}
  virtual void WriteDescriptor(const DescriptorTable* table, ezUInt32 rangeIndex, ezUInt32 arrayIndex, const Sampler* sampler) {}

  virtual void Map(const GPUResource* resource, Mapping* mapping) = 0;
  virtual void Unmap(const GPUResource* resource) = 0;
  virtual bool QueryRead(const GPUQuery* query, GPUQueryResult* result) = 0;

  virtual void SetName(GPUResource* pResource, const char* name) = 0;

  virtual void PresentBegin(CommandList cmd) = 0;
  virtual void PresentEnd(CommandList cmd) = 0;

  virtual CommandList BeginCommandList() = 0;
  virtual void SubmitCommandLists() = 0;

  virtual void WaitForGPU() = 0;
  virtual void ClearPipelineStateCache(){};

  inline bool GetVSyncEnabled() const { return VSYNC; }
  virtual void SetVSyncEnabled(bool value) { VSYNC = value; }
  inline ezUInt64 GetFrameCount() const { return FRAMECOUNT; }
  inline ezUInt64 GetFrameIndex() const { return FRAMECOUNT % BACKBUFFER_COUNT; }

  // Returns native resolution width of back buffer in pixels:
  inline int GetResolutionWidth() const { return RESOLUTIONWIDTH; }
  // Returns native resolution height of back buffer in pixels:
  inline int GetResolutionHeight() const { return RESOLUTIONHEIGHT; }

  // Returns the width of the screen with DPI scaling applied (subpixel size):
  float GetScreenWidth() const;
  // Returns the height of the screen with DPI scaling applied (subpixel size):
  float GetScreenHeight() const;


  virtual void SetResolution(int width, int height) = 0;

  virtual Texture GetBackBuffer() = 0;

  bool CheckCapability(GRAPHICSDEVICE_CAPABILITY capability) const;

  ezUInt32 GetFormatStride(ezEnum<ezRHIFormat> value) const;
  bool IsFormatUnorm(ezEnum<ezRHIFormat> value) const;
  bool IsFormatBlockCompressed(ezEnum<ezRHIFormat> value) const;
  bool IsFormatStencilSupport(ezEnum<ezRHIFormat> value) const;

  inline ezMat4 GetScreenProjection() const
  {
    return ezGraphicsUtils::CreateOrthographicProjectionMatrix(0, (float)GetScreenWidth(), (float)GetScreenHeight(), 0, -1, 1);
    //return XMMatrixOrthographicOffCenterLH(0, (float)GetScreenWidth(), (float)GetScreenHeight(), 0, -1, 1);
  }
  inline ezEnum<ezRHIFormat> GetBackBufferFormat() const { return BACKBUFFER_FORMAT; }
  static constexpr ezUInt32 GetBackBufferCount() { return BACKBUFFER_COUNT; }

  inline bool IsDebugDevice() const { return DEBUGDEVICE; }

  inline size_t GetShaderIdentifierSize() const { return SHADER_IDENTIFIER_SIZE; }
  inline size_t GetTopLevelAccelerationStructureInstanceSize() const { return TOPLEVEL_ACCELERATION_STRUCTURE_INSTANCE_SIZE; }
  inline ezUInt32 GetVariableRateShadingTileSize() const { return VARIABLE_RATE_SHADING_TILE_SIZE; }

  ///////////////Thread-sensitive////////////////////////

  virtual void RenderPassBegin(const RenderPass* renderpass, CommandList cmd) = 0;
  virtual void RenderPassEnd(CommandList cmd) = 0;
  virtual void BindScissorRects(ezUInt32 numRects, const Rect* rects, CommandList cmd) = 0;
  virtual void BindViewports(ezUInt32 NumViewports, const Viewport* pViewports, CommandList cmd) = 0;
  virtual void BindResource(ezEnum<ezRHIShaderStage> stage, const GPUResource* resource, ezUInt32 slot, CommandList cmd, int subresource = -1) = 0;
  virtual void BindResources(ezEnum<ezRHIShaderStage> stage, const GPUResource* const* resources, ezUInt32 slot, ezUInt32 count, CommandList cmd) = 0;
  virtual void BindUAV(ezEnum<ezRHIShaderStage> stage, const GPUResource* resource, ezUInt32 slot, CommandList cmd, int subresource = -1) = 0;
  virtual void BindUAVs(ezEnum<ezRHIShaderStage> stage, const GPUResource* const* resources, ezUInt32 slot, ezUInt32 count, CommandList cmd) = 0;
  virtual void UnbindResources(ezUInt32 slot, ezUInt32 num, CommandList cmd) = 0;
  virtual void UnbindUAVs(ezUInt32 slot, ezUInt32 num, CommandList cmd) = 0;
  virtual void BindSampler(ezEnum<ezRHIShaderStage> stage, const Sampler* sampler, ezUInt32 slot, CommandList cmd) = 0;
  virtual void BindConstantBuffer(ezEnum<ezRHIShaderStage> stage, const GPUBuffer* buffer, ezUInt32 slot, CommandList cmd) = 0;
  virtual void BindVertexBuffers(const GPUBuffer* const* vertexBuffers, ezUInt32 slot, ezUInt32 count, const ezUInt32* strides, const ezUInt32* offsets, CommandList cmd) = 0;
  virtual void BindIndexBuffer(const GPUBuffer* indexBuffer, const ezEnum<ezRHIIndexBufferFormat> format, ezUInt32 offset, CommandList cmd) = 0;
  virtual void BindStencilRef(ezUInt32 value, CommandList cmd) = 0;
  virtual void BindBlendFactor(float r, float g, float b, float a, CommandList cmd) = 0;
  virtual void BindShadingRate(SHADING_RATE rate, CommandList cmd) {}
  virtual void BindShadingRateImage(const Texture* texture, CommandList cmd) {}
  virtual void BindPipelineState(const PipelineState* pso, CommandList cmd) = 0;
  virtual void BindComputeShader(const Shader* cs, CommandList cmd) = 0;
  virtual void Draw(ezUInt32 vertexCount, ezUInt32 startVertexLocation, CommandList cmd) = 0;
  virtual void DrawIndexed(ezUInt32 indexCount, ezUInt32 startIndexLocation, ezUInt32 baseVertexLocation, CommandList cmd) = 0;
  virtual void DrawInstanced(ezUInt32 vertexCount, ezUInt32 instanceCount, ezUInt32 startVertexLocation, ezUInt32 startInstanceLocation, CommandList cmd) = 0;
  virtual void DrawIndexedInstanced(ezUInt32 indexCount, ezUInt32 instanceCount, ezUInt32 startIndexLocation, ezUInt32 baseVertexLocation, ezUInt32 startInstanceLocation, CommandList cmd) = 0;
  virtual void DrawInstancedIndirect(const GPUBuffer* args, ezUInt32 args_offset, CommandList cmd) = 0;
  virtual void DrawIndexedInstancedIndirect(const GPUBuffer* args, ezUInt32 args_offset, CommandList cmd) = 0;
  virtual void Dispatch(ezUInt32 threadGroupCountX, ezUInt32 threadGroupCountY, ezUInt32 threadGroupCountZ, CommandList cmd) = 0;
  virtual void DispatchIndirect(const GPUBuffer* args, ezUInt32 args_offset, CommandList cmd) = 0;
  virtual void DispatchMesh(ezUInt32 threadGroupCountX, ezUInt32 threadGroupCountY, ezUInt32 threadGroupCountZ, CommandList cmd) {}
  virtual void DispatchMeshIndirect(const GPUBuffer* args, ezUInt32 args_offset, CommandList cmd) {}
  virtual void CopyResource(const GPUResource* pDst, const GPUResource* pSrc, CommandList cmd) = 0;
  virtual void UpdateBuffer(const GPUBuffer* buffer, const void* data, CommandList cmd, int dataSize = -1) = 0;
  virtual void QueryBegin(const GPUQuery* query, CommandList cmd) = 0;
  virtual void QueryEnd(const GPUQuery* query, CommandList cmd) = 0;
  virtual void Barrier(const GPUBarrier* barriers, ezUInt32 numBarriers, CommandList cmd) = 0;
  virtual void BuildRaytracingAccelerationStructure(const RaytracingAccelerationStructure* dst, CommandList cmd, const RaytracingAccelerationStructure* src = nullptr) {}
  virtual void BindRaytracingPipelineState(const RaytracingPipelineState* rtpso, CommandList cmd) {}
  virtual void DispatchRays(const DispatchRaysDesc* desc, CommandList cmd) {}

  virtual void BindDescriptorTable(BINDPOINT bindpoint, ezUInt32 space, const DescriptorTable* table, CommandList cmd) {}
  virtual void BindRootDescriptor(BINDPOINT bindpoint, ezUInt32 index, const GPUBuffer* buffer, ezUInt32 offset, CommandList cmd) {}
  virtual void BindRootConstants(BINDPOINT bindpoint, ezUInt32 index, const void* srcdata, CommandList cmd) {}

  struct GPUAllocation
  {
    void* data = nullptr;              // application can write to this. Reads might be not supported or slow. The offset is already applied
    const GPUBuffer* buffer = nullptr; // application can bind it to the GPU
    ezUInt32 offset = 0;               // allocation's offset from the GPUbuffer's beginning

    // Returns true if the allocation was successful
    inline bool IsValid() const { return data != nullptr && buffer != nullptr; }
  };
  // Allocates temporary memory that the CPU can write and GPU can read.
  //	It is only alive for one frame and automatically invalidated after that.
  //	The CPU pointer gets invalidated as soon as there is a Draw() or Dispatch() event on the same thread
  //	This allocation can be used to provide temporary vertex buffer, index buffer or raw buffer data to shaders
  virtual GPUAllocation AllocateGPU(size_t dataSize, CommandList cmd) = 0;

  virtual void EventBegin(const char* name, CommandList cmd) = 0;
  virtual void EventEnd(CommandList cmd) = 0;
  virtual void SetMarker(const char* name, CommandList cmd) = 0;
};
