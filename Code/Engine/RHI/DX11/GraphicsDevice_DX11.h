#pragma once

#if EZ_RHI_D3D11_SUPPORTED
#  include <RHI/GraphicsDevice.h>
#  include <RHI/RHIDLL.h>
#  include <RHI/RHIPCH.h>
#include <Foundation/Types/SharedPtr.h>

#  include <DXGI1_3.h>
#  include <d3d11_3.h>
#  include <wrl/client.h> // ComPtr

#  include <atomic>

EZ_DEFINE_AS_POD_TYPE(D3D11_SUBRESOURCE_DATA);
EZ_DEFINE_AS_POD_TYPE(D3D11_INPUT_ELEMENT_DESC);

class EZ_RHI_DLL GraphicsDevice_DX11 : public GraphicsDevice
{
private:
  D3D_DRIVER_TYPE driverType;
  D3D_FEATURE_LEVEL featureLevel;
  Microsoft::WRL::ComPtr<ID3D11Device> device;
  Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
  Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
  Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContexts[COMMANDLIST_COUNT];
  Microsoft::WRL::ComPtr<ID3D11CommandList> commandLists[COMMANDLIST_COUNT];
  Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation> userDefinedAnnotations[COMMANDLIST_COUNT];

  ezUInt32 stencilRef[COMMANDLIST_COUNT];
  ezVec4 blendFactor[COMMANDLIST_COUNT];

  ID3D11VertexShader* prev_vs[COMMANDLIST_COUNT] = {};
  ID3D11PixelShader* prev_ps[COMMANDLIST_COUNT] = {};
  ID3D11HullShader* prev_hs[COMMANDLIST_COUNT] = {};
  ID3D11DomainShader* prev_ds[COMMANDLIST_COUNT] = {};
  ID3D11GeometryShader* prev_gs[COMMANDLIST_COUNT] = {};
  ID3D11ComputeShader* prev_cs[COMMANDLIST_COUNT] = {};
  ezVec4 prev_blendfactor[COMMANDLIST_COUNT] = {};
  ezUInt32 prev_samplemask[COMMANDLIST_COUNT] = {};
  ID3D11BlendState* prev_bs[COMMANDLIST_COUNT] = {};
  ID3D11RasterizerState* prev_rs[COMMANDLIST_COUNT] = {};
  ezUInt32 prev_stencilRef[COMMANDLIST_COUNT] = {};
  ID3D11DepthStencilState* prev_dss[COMMANDLIST_COUNT] = {};
  ID3D11InputLayout* prev_il[COMMANDLIST_COUNT] = {};
  ezRHIPrimitiveTopology::Enum prev_pt[COMMANDLIST_COUNT] = {};

  const PipelineState* active_pso[COMMANDLIST_COUNT] = {};
  bool dirty_pso[COMMANDLIST_COUNT] = {};
  void pso_validate(CommandList cmd);

  const RenderPass* active_renderpass[COMMANDLIST_COUNT] = {};

  ID3D11UnorderedAccessView* raster_uavs[COMMANDLIST_COUNT][8] = {};
  ezUInt8 raster_uavs_slot[COMMANDLIST_COUNT] = {};
  ezUInt8 raster_uavs_count[COMMANDLIST_COUNT] = {};
  void validate_raster_uavs(CommandList cmd);

  struct GPUAllocator
  {
    GPUBuffer buffer;
    size_t byteOffset = 0;
    ezUInt64 residentFrame = 0;
    bool dirty = false;
  } frame_allocators[COMMANDLIST_COUNT];
  void commit_allocations(CommandList cmd);

  void CreateBackBufferResources();

  std::atomic<CommandList> cmd_count{0};

  struct EmptyResourceHandle : public ezRefCounted
  {
  }; // only care about control-block
  ezSharedPtr<EmptyResourceHandle> emptyresource;

public:
  GraphicsDevice_DX11(RHIWindowType window, bool fullscreen = false, bool debuglayer = false);

  bool CreateBuffer(const GPUBufferDesc* pDesc, const SubresourceData* pInitialData, GPUBuffer* pBuffer) override;
  bool CreateTexture(const TextureDesc* pDesc, const SubresourceData* pInitialData, Texture* pTexture) override;
  bool CreateInputLayout(const InputLayoutDesc* pInputElementDescs, ezUInt32 NumElements, const Shader* shader, InputLayout* pInputLayout) override;
  bool CreateShader(ezEnum<ezRHIShaderStage> stage, const void* pShaderBytecode, size_t BytecodeLength, Shader* pShader) override;
  bool CreateBlendState(const BlendStateDesc* pBlendStateDesc, BlendState* pBlendState) override;
  bool CreateDepthStencilState(const DepthStencilStateDesc* pDepthStencilStateDesc, DepthStencilState* pDepthStencilState) override;
  bool CreateRasterizerState(const RasterizerStateDesc* pRasterizerStateDesc, RasterizerState* pRasterizerState) override;
  bool CreateSampler(const SamplerDesc* pSamplerDesc, Sampler* pSamplerState) override;
  bool CreateQuery(const GPUQueryDesc* pDesc, GPUQuery* pQuery) override;
  bool CreatePipelineState(const PipelineStateDesc* pDesc, PipelineState* pso) override;
  bool CreateRenderPass(const RenderPassDesc* pDesc, RenderPass* renderpass) override;

  int CreateSubresource(Texture* texture, SUBRESOURCE_TYPE type, ezUInt32 firstSlice, ezUInt32 sliceCount, ezUInt32 firstMip, ezUInt32 mipCount) override;
  int CreateSubresource(GPUBuffer* buffer, SUBRESOURCE_TYPE type, ezUInt64 offset, ezUInt64 size = ~0) override;

  void Map(const GPUResource* resource, Mapping* mapping) override;
  void Unmap(const GPUResource* resource) override;
  bool QueryRead(const GPUQuery* query, GPUQueryResult* result) override;

  void SetName(GPUResource* pResource, const char* name) override;

  void PresentBegin(CommandList cmd) override;
  void PresentEnd(CommandList cmd) override;

  void WaitForGPU() override;

  CommandList BeginCommandList() override;
  void SubmitCommandLists() override;

  void SetResolution(int width, int height) override;

  Texture GetBackBuffer() override;

  ///////////////Thread-sensitive////////////////////////

  void RenderPassBegin(const RenderPass* renderpass, CommandList cmd) override;
  void RenderPassEnd(CommandList cmd) override;
  void BindScissorRects(ezUInt32 numRects, const Rect* rects, CommandList cmd) override;
  void BindViewports(ezUInt32 NumViewports, const Viewport* pViewports, CommandList cmd) override;
  void BindResource(ezEnum<ezRHIShaderStage> stage, const GPUResource* resource, ezUInt32 slot, CommandList cmd, int subresource = -1) override;
  void BindResources(ezEnum<ezRHIShaderStage> stage, const GPUResource* const* resources, ezUInt32 slot, ezUInt32 count, CommandList cmd) override;
  void BindUAV(ezEnum<ezRHIShaderStage> stage, const GPUResource* resource, ezUInt32 slot, CommandList cmd, int subresource = -1) override;
  void BindUAVs(ezEnum<ezRHIShaderStage> stage, const GPUResource* const* resources, ezUInt32 slot, ezUInt32 count, CommandList cmd) override;
  void UnbindResources(ezUInt32 slot, ezUInt32 num, CommandList cmd) override;
  void UnbindUAVs(ezUInt32 slot, ezUInt32 num, CommandList cmd) override;
  void BindSampler(ezEnum<ezRHIShaderStage> stage, const Sampler* sampler, ezUInt32 slot, CommandList cmd) override;
  void BindConstantBuffer(ezEnum<ezRHIShaderStage> stage, const GPUBuffer* buffer, ezUInt32 slot, CommandList cmd) override;
  void BindVertexBuffers(const GPUBuffer* const* vertexBuffers, ezUInt32 slot, ezUInt32 count, const ezUInt32* strides, const ezUInt32* offsets, CommandList cmd) override;
  void BindIndexBuffer(const GPUBuffer* indexBuffer, const ezEnum<ezRHIIndexBufferFormat> format, ezUInt32 offset, CommandList cmd) override;
  void BindStencilRef(ezUInt32 value, CommandList cmd) override;
  void BindBlendFactor(float r, float g, float b, float a, CommandList cmd) override;
  void BindPipelineState(const PipelineState* pso, CommandList cmd) override;
  void BindComputeShader(const Shader* cs, CommandList cmd) override;
  void Draw(ezUInt32 vertexCount, ezUInt32 startVertexLocation, CommandList cmd) override;
  void DrawIndexed(ezUInt32 indexCount, ezUInt32 startIndexLocation, ezUInt32 baseVertexLocation, CommandList cmd) override;
  void DrawInstanced(ezUInt32 vertexCount, ezUInt32 instanceCount, ezUInt32 startVertexLocation, ezUInt32 startInstanceLocation, CommandList cmd) override;
  void DrawIndexedInstanced(ezUInt32 indexCount, ezUInt32 instanceCount, ezUInt32 startIndexLocation, ezUInt32 baseVertexLocation, ezUInt32 startInstanceLocation, CommandList cmd) override;
  void DrawInstancedIndirect(const GPUBuffer* args, ezUInt32 args_offset, CommandList cmd) override;
  void DrawIndexedInstancedIndirect(const GPUBuffer* args, ezUInt32 args_offset, CommandList cmd) override;
  void Dispatch(ezUInt32 threadGroupCountX, ezUInt32 threadGroupCountY, ezUInt32 threadGroupCountZ, CommandList cmd) override;
  void DispatchIndirect(const GPUBuffer* args, ezUInt32 args_offset, CommandList cmd) override;
  void CopyResource(const GPUResource* pDst, const GPUResource* pSrc, CommandList cmd) override;
  void UpdateBuffer(const GPUBuffer* buffer, const void* data, CommandList cmd, int dataSize = -1) override;
  void QueryBegin(const GPUQuery* query, CommandList cmd) override;
  void QueryEnd(const GPUQuery* query, CommandList cmd) override;
  void Barrier(const GPUBarrier* barriers, ezUInt32 numBarriers, CommandList cmd) override {}

  GPUAllocation AllocateGPU(size_t dataSize, CommandList cmd) override;

  void EventBegin(const char* name, CommandList cmd) override;
  void EventEnd(CommandList cmd) override;
  void SetMarker(const char* name, CommandList cmd) override;
};

#endif // WICKEDENGINE_BUILD_DX11
