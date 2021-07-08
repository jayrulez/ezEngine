#include <RHI/RHIDLL.h>

#include <RHI/Basics.h>

struct EZ_RHI_DLL ezRHILazyViewDesc
{
  ezUInt32 m_Level = 0;
  ezUInt32 m_Count = static_cast<ezUInt32>(-1);
  ezRHIResourceFormat::Enum m_BufferFormat = ezRHIResourceFormat::Unknown;
};

/*
struct ezRHIResourceLazyViewDesc;

class ezRHIDeferredView
{
public:
  virtual ~ezRHIDeferredView() = default;
  virtual ezRHIResourceLazyViewDesc* GetView(ezRHIRenderCommandList& commandList) = 0;
  virtual void OnDestroy(ezRHIResourceLazyViewDesc& viewDesc) = 0;
};

struct ezRHIResourceLazyViewDesc
{
  ezRHIResourceLazyViewDesc(ezRHIDeferredView& deferred_view, ezRHIResource* resource)
    : m_DeferredView(deferred_view)
    , m_Resource(resource)
  {
  }

  ~ezRHIResourceLazyViewDesc()
  {
    m_DeferredView.OnDestroy(*this);
  }

  ezRHIResource* m_Resource;
  ezRHILazyViewDesc m_ViewDesc;
  ezRHIDeferredView& m_DeferredView;
};
*/

struct ezRHIRenderPassBeginColorDescription
{
  ezRHIResource* m_Texture;
  ezRHILazyViewDesc m_ViewDesc;
  ezRHIRenderPassLoadOp m_LoadOp = ezRHIRenderPassLoadOp::Clear;
  ezRHIRenderPassStoreOp m_StoreOp = ezRHIRenderPassStoreOp::Store;
  ezColor m_ClearColor = {};
};

struct ezRHIRenderPassBeginDepthStencilDescription
{
  ezRHIResource* m_Texture;
  ezRHILazyViewDesc m_ViewDesc;
  ezRHIRenderPassLoadOp m_DepthLoadOp = ezRHIRenderPassLoadOp::Clear;
  ezRHIRenderPassStoreOp m_DepthStoreOp = ezRHIRenderPassStoreOp::Store;
  ezRHIRenderPassLoadOp m_StencilLoadOp = ezRHIRenderPassLoadOp::Clear;
  ezRHIRenderPassStoreOp m_StencilStoreOp = ezRHIRenderPassStoreOp::Store;
  float m_ClearDepth = 1.0f;
  ezUInt32 m_ClearStencil = 0;
};

struct ezRHIRenderPassBeginDescription
{
  ezStaticArray<ezRHIRenderPassBeginColorDescription, 8> m_Colors;
  ezRHIRenderPassBeginDepthStencilDescription m_DepthStencil;
};
