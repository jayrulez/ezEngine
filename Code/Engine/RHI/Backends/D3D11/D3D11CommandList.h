#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Resources/BoundResourceSetInfo.h>
#include <RHI/Resources/CommandList.h>

#include <d3dcommon.h>

struct ID3D11Buffer;
struct ID3D11DeviceContext;
struct ID3D11DeviceContext1;
struct ID3DUserDefinedAnnotation;
struct ID3D11CommandList;
struct D3D11_VIEWPORT;
struct ID3D11BlendState;
struct ID3D11DepthStencilState;
struct ID3D11RasterizerState;
struct ID3D11InputLayout;
struct ID3D11VertexShader;
struct ID3D11GeometryShader;
struct ID3D11HullShader;
struct ID3D11DomainShader;
struct ID3D11PixelShader;


struct RawRect
{
  ezUInt32 left;
  ezUInt32 top;
  ezUInt32 right;
  ezUInt32 bottom;
};

class D3D11TextureView;
class D3D11Sampler;
class D3D11Pipeline;
class D3D11GraphicsDevice;
class D3D11Framebuffer;
class D3D11Swapchain;

class D3D11CommandList : public RHICommandList
{
private:
  struct D3D11BufferRange : public ezHashableStruct<D3D11BufferRange>
  {
    D3D11DeviceBuffer* Buffer;
    ezUInt32 Offset;
    ezUInt32 Size;

    bool IsFullRange();

    D3D11BufferRange(D3D11DeviceBuffer* buffer, ezUInt32 offset, ezUInt32 size);

    bool operator==(const D3D11BufferRange& other)
    {
      return Buffer == other.Buffer && Offset == other.Offset && Size == other.Size;
    }
  };

  struct BoundTextureInfo
  {
    ezUInt32 Slot = 0;
    ezBitflags<RHIShaderStages> Stages;
    ezUInt32 ResourceSet = 0;

    BoundTextureInfo() = default;
    BoundTextureInfo(ezUInt32 slot, ezBitflags<RHIShaderStages> stages, ezUInt32 resourceSet);
  };

private:
  D3D11GraphicsDevice* GraphicsDevice = nullptr;
  ID3D11DeviceContext* Context = nullptr;
  ID3D11DeviceContext1* Context1 = nullptr;
  ID3DUserDefinedAnnotation* UDA = nullptr;
  bool Begun = false;
  ID3D11CommandList* CommandList = nullptr;

  ezDynamicArray<D3D11_VIEWPORT> Viewports; // = new Vortice.Mathematics.Viewport[0];
  ezDynamicArray<RawRect> Scissors;
  bool ViewportsChanged = false;
  bool ScissorRectsChanged = false;

  ezUInt32 NumVertexBindings = 0;
  ezStaticArray<ID3D11Buffer*, 1> VertexBindings;
  ezDynamicArray<ezUInt32> VertexStrides;
  ezStaticArray<ezUInt32, 1> VertexOffsets;

  // Cached pipeline State
  RHIDeviceBuffer* _ib = nullptr;
  ezUInt32 _ibOffset = 0;
  ID3D11BlendState* BlendState = nullptr;
  ID3D11DepthStencilState* DepthStencilState = nullptr;
  ezUInt32 StencilReference = 0;
  ID3D11RasterizerState* RasterizerState = nullptr;
  D3D_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
  ID3D11InputLayout* InputLayout = nullptr;
  ID3D11VertexShader* VertexShader = nullptr;
  ID3D11GeometryShader* GeometryShader = nullptr;
  ID3D11HullShader* HullShader = nullptr;
  ID3D11DomainShader* DomainShader = nullptr;
  ID3D11PixelShader* PixelShader = nullptr;

  D3D11Pipeline* GraphicsPipeline = nullptr;
  ezHybridArray<RHIBoundResourceSetInfo, 1> GraphicsResourceSets;
  // Resource sets are invalidated when a new resource set is bound with an incompatible SRV or UAV.
  ezHybridArray<bool, 1> InvalidatedGraphicsResourceSets;

  D3D11Pipeline* ComputePipeline = nullptr;
  ezHybridArray<RHIBoundResourceSetInfo, 1> ComputeResourceSets;
  // Resource sets are invalidated when a new resource set is bound with an incompatible SRV or UAV.
  ezStaticArray<bool, 1> InvalidatedComputeResourceSets;
  bool VertexBindingsChanged = false;
  ezStaticArray<ID3D11Buffer*, 1> CBOut;
  ezStaticArray<ezUInt32, 1> FirstConstRef;
  ezStaticArray<ezUInt32, 1> NumConstsRef;

  // Cached resources
  static constexpr ezUInt32 MaxCachedUniformBuffers = 15;
  ezStaticArray<D3D11BufferRange*, MaxCachedUniformBuffers> VertexBoundUniformBuffers;
  ezStaticArray<D3D11BufferRange*, MaxCachedUniformBuffers> FragmentBoundUniformBuffers;
  static constexpr ezUInt32 MaxCachedTextureViews = 16;
  ezStaticArray<D3D11TextureView*, MaxCachedTextureViews> VertexBoundTextureViews;
  ezStaticArray<D3D11TextureView*, MaxCachedTextureViews> FragmentBoundTextureViews;
  static constexpr ezUInt32 MaxCachedSamplers = 4;
  ezStaticArray<D3D11Sampler*, MaxCachedSamplers> VertexBoundSamplers;
  ezStaticArray<D3D11Sampler*, MaxCachedSamplers> FragmentBoundSamplers;

  ezHashTable<RHITexture*, ezDynamicArray<BoundTextureInfo>> BoundSRVs;
  ezHashTable<RHITexture*, ezDynamicArray<BoundTextureInfo>> BoundUAVs;
  ezHybridArray<ezDynamicArray<BoundTextureInfo>, 20> BoundTextureInfoPool;

  static constexpr ezUInt32 MaxUAVs = 8;
  ezHybridArray<std::tuple<RHIDeviceBuffer*, int>, MaxUAVs> BoundComputeUAVBuffers;
  ezHybridArray<std::tuple<RHIDeviceBuffer*, int>, MaxUAVs> BoundOMUAVBuffers;

  ezDynamicArray<D3D11DeviceBuffer*> AvailableStagingBuffers;
  ezDynamicArray<D3D11DeviceBuffer*> SubmittedStagingBuffers;

  ezDynamicArray<D3D11Swapchain*> ReferencedSwapchains;

  bool Disposed = false;
  ezString Name;

public:
  virtual ezString GetName() const override;

  virtual void SetName(const ezString& name) override;
  virtual bool IsDisposed() const override;
  virtual void Dispose() override;

protected:
  virtual void BeginCore() override;
  virtual void EndCore() override;

  virtual void SetPipelineCore(RHIPipeline* pipeline) override;
  virtual void SetVertexBufferCore(ezUInt32 index, RHIDeviceBuffer* buffer, ezUInt32 offset) override;

  virtual void SetIndexBufferCore(RHIDeviceBuffer* buffer, ezEnum<RHIIndexFormat> format, ezUInt32 offset) override;

  virtual void SetGraphicsResourceSetCore(ezUInt32 slot, RHIResourceSet* resourceSet, ezUInt32 dynamicOffsetsCount, const ezDynamicArray<ezUInt32>& dynamicOffsets) override;

  virtual void SetComputeResourceSetCore(ezUInt32 slot, RHIResourceSet* resourceSet, ezUInt32 dynamicOffsetsCount, const ezDynamicArray<ezUInt32>& dynamicOffsets) override;

  virtual void SetFramebufferCore(RHIFramebuffer* framebuffer) override;

  virtual void ClearColorTargetCore(ezUInt32 index, ezColor clearColor) override;

  virtual void ClearDepthStencilCore(float depth, ezUInt8 stencil) override;
  virtual void SetViewportCore(ezUInt32 index, const RHIViewport& viewport) override;
  virtual void SetScissorRectCore(ezUInt32 index, ezUInt32 x, ezUInt32 y, ezUInt32 width, ezUInt32 height) override;
  virtual void DrawCore(ezUInt32 vertexCount, ezUInt32 instanceCount, ezUInt32 vertexStart, ezUInt32 instanceStart) override;

  virtual void DrawIndexedCore(ezUInt32 indexCount, ezUInt32 instanceCount, ezUInt32 indexStart, ezInt32 vertexOffset, ezUInt32 instanceStart) override;
  virtual void DrawIndirectCore(RHIDeviceBuffer* indirectBuffer, ezUInt32 offset, ezUInt32 drawCount, ezUInt32 stride) override;

  virtual void DrawIndexedIndirectCore(RHIDeviceBuffer* indirectBuffer, ezUInt32 offset, ezUInt32 drawCount, ezUInt32 stride) override;

  virtual void DispatchCore(ezUInt32 groupCountX, ezUInt32 groupCountY, ezUInt32 groupCountZ) override;

  virtual void DispatchIndirectCore(RHIDeviceBuffer* indirectBuffer, ezUInt32 offset) override;

  virtual void ResolveTextureCore(RHITexture* source, RHITexture* destination) override;

  virtual void UpdateBufferCore(RHIDeviceBuffer* buffer, ezUInt32 bufferOffset, ezUInt8* source, ezUInt32 size) override;

  virtual void CopyBufferCore(RHIDeviceBuffer* source, ezUInt32 sourceOffset, RHIDeviceBuffer* destination, ezUInt32 destinationOffset, ezUInt32 size) override;

  virtual void CopyTextureCore(
    RHITexture* source,
    ezUInt32 srcX, ezUInt32 srcY, ezUInt32 srcZ,
    ezUInt32 srcMipLevel,
    ezUInt32 srcBaseArrayLayer,
    RHITexture* destination,
    ezUInt32 dstX, ezUInt32 dstY, ezUInt32 dstZ,
    ezUInt32 dstMipLevel,
    ezUInt32 dstBaseArrayLayer,
    ezUInt32 width, ezUInt32 height, ezUInt32 depth,
    ezUInt32 layerCount) override;

  virtual void GenerateMipmapsCore(RHITexture* texture) override;

  virtual void PushDebugGroupCore(ezString name) override;
  virtual void PopDebugGroupCore() override;
  virtual void InsertDebugMarkerCore(ezString name) override;

public:
  D3D11CommandList(D3D11GraphicsDevice* graphicsDevice, const RHICommandListDescription& description);

  ID3D11CommandList* GetDeviceCommandList() const;

  //TODO: private friend
  ID3D11DeviceContext* GetDeviceContext() const;

  void Reset();

private:
  D3D11Framebuffer* GetD3D11Framebuffer() const;

  void ClearState();

  void ResetManagedState();

  void ClearSets(ezDynamicArray<RHIBoundResourceSetInfo>& boundSets);

  void UnbindUAVTexture(RHITexture* target);

  void UnbindUAVBuffer(RHIDeviceBuffer* buffer);

  void UnbindUAVBufferIndividual(RHIDeviceBuffer* buffer, bool compute);

  void TrackBoundUAVBuffer(RHIDeviceBuffer* buffer, ezUInt32 slot, bool compute);

  void BindUnorderedAccessView(
    RHITexture* texture,
    RHIDeviceBuffer* buffer,
    ID3D11UnorderedAccessView* uav,
    ezUInt32 slot,
    ezBitflags<RHIShaderStages> stages,
    ezUInt32 resourceSet);

  void PackRangeParams(D3D11BufferRange* range);

  void BindStorageBufferView(D3D11BufferRange* range, int slot, ezBitflags<RHIShaderStages> stages);

  void BindUniformBuffer(D3D11BufferRange* range, ezUInt32 slot, ezBitflags<RHIShaderStages> stages);

  void BindSampler(D3D11Sampler* sampler, ezUInt32 slot, ezBitflags<RHIShaderStages> stages);

  void BindTextureView(D3D11TextureView* texView, ezUInt32 slot, ezBitflags<RHIShaderStages> stages, ezUInt32 resourceSet);

  void UpdateSubresource_Workaround(
    ID3D11Resource* resource,
    int subresource,
    D3D11_BOX region,
    ezUInt8* data);


  D3D11DeviceBuffer* GetFreeStagingBuffer(ezUInt32 size);

  ezDynamicArray<BoundTextureInfo> GetNewOrCachedBoundTextureInfoList();

  void OnCompleted();

  void FlushVertexBindings();

  void FlushScissorRects();

  void FlushViewports();

  void PreDispatchCommand();

  void PreDrawCommand();

  void ActivateResourceSet(ezUInt32 slot, RHIBoundResourceSetInfo brsi, bool graphics);

  D3D11BufferRange* GetBufferRange(RHIDeviceResource* resource, ezUInt32 additionalOffset);

  void UnbindSRVTexture(RHITexture* target);

  void PoolBoundTextureList(ezDynamicArray<BoundTextureInfo> btis)
  {
    BoundTextureInfoPool.PushBack(btis);
  }


  ezUInt32 GetConstantBufferBase(ezUInt32 slot, bool graphics);

  ezUInt32 GetUnorderedAccessBase(ezUInt32 slot, bool graphics);

  ezUInt32 GetTextureBase(ezUInt32 slot, bool graphics);

  ezUInt32 GetSamplerBase(ezUInt32 slot, bool graphics);
};
