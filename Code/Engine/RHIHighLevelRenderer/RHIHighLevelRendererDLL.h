#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Instance/BaseTypes.h>

#include <Foundation/Basics.h>
#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Types/Id.h>
#include <Foundation/Types/RefCounted.h>

// Configure the DLL Import/Export Define
#if EZ_ENABLED(EZ_COMPILE_ENGINE_AS_DLL)
#  ifdef BUILDSYSTEM_BUILDING_RHIHIGHLEVELRENDERER_LIB
#    define EZ_RHIHIGHLEVELRENDERER_DLL __declspec(dllexport)
#  else
#    define EZ_RHIHIGHLEVELRENDERER_DLL __declspec(dllimport)
#  endif
#else
#  define EZ_RHIHIGHLEVELRENDERER_DLL
#endif

class RenderCommandList;
struct ResourceLazyViewDesc;

struct EZ_RHIHIGHLEVELRENDERER_DLL LazyViewDesc
{
  ezUInt32 level = 0;
  ezUInt32 count = static_cast<ezUInt32>(-1);
  ezRHIResourceFormat::Enum buffer_format = ezRHIResourceFormat::UNKNOWN;

  auto MakeTie() const
  {
    return std::tie(level, count, buffer_format);
  }
};

struct EZ_RHIHIGHLEVELRENDERER_DLL RenderPassBeginColorDesc
{
  std::shared_ptr<Resource> texture;
  LazyViewDesc view_desc;
  RenderPassLoadOp load_op = RenderPassLoadOp::kClear;
  RenderPassStoreOp store_op = RenderPassStoreOp::kStore;
  ezColor clear_color = {};
};

struct EZ_RHIHIGHLEVELRENDERER_DLL RenderPassBeginDepthStencilDesc
{
  std::shared_ptr<Resource> texture;
  LazyViewDesc view_desc;
  RenderPassLoadOp depth_load_op = RenderPassLoadOp::kClear;
  RenderPassStoreOp depth_store_op = RenderPassStoreOp::kStore;
  RenderPassLoadOp stencil_load_op = RenderPassLoadOp::kClear;
  RenderPassStoreOp stencil_store_op = RenderPassStoreOp::kStore;
  float clear_depth = 1.0f;
  uint8_t clear_stencil = 0;
};

struct EZ_RHIHIGHLEVELRENDERER_DLL RenderPassBeginDesc
{
  std::array<RenderPassBeginColorDesc, 8> colors = {};
  RenderPassBeginDepthStencilDesc depth_stencil;
};

class EZ_RHIHIGHLEVELRENDERER_DLL DeferredView
{
public:
  virtual ~DeferredView() = default;
  virtual std::shared_ptr<ResourceLazyViewDesc> GetView(RenderCommandList& command_list) = 0;
  virtual void OnDestroy(ResourceLazyViewDesc& view_desc) = 0;
};

struct EZ_RHIHIGHLEVELRENDERER_DLL ResourceLazyViewDesc
{
  ResourceLazyViewDesc(DeferredView& deferred_view, const std::shared_ptr<Resource>& resource)
    : m_deferred_view(deferred_view)
    , resource(resource)
  {
  }

  ~ResourceLazyViewDesc()
  {
    m_deferred_view.OnDestroy(*this);
  }

  std::shared_ptr<Resource> resource;
  LazyViewDesc view_desc;
  DeferredView& m_deferred_view;
};
