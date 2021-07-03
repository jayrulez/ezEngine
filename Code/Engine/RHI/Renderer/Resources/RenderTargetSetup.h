
#pragma once

#include <Foundation/Basics.h>
#include <RHI/Renderer/Basics.h>

// \brief This class can be used to construct render target setups on the stack.
class EZ_RHI_DLL ezRHIRenderTargetSetup
{
public:
  ezRHIRenderTargetSetup();

  ezRHIRenderTargetSetup& SetRenderTarget(ezUInt8 uiIndex, ezRHIRenderTargetViewHandle hRenderTarget);
  ezRHIRenderTargetSetup& SetDepthStencilTarget(ezRHIRenderTargetViewHandle hDSTarget);

  bool operator==(const ezRHIRenderTargetSetup& other) const;
  bool operator!=(const ezRHIRenderTargetSetup& other) const;

  inline bool HasRenderTargets() const;

  inline ezUInt8 GetMaxRenderTargetIndex() const;

  inline ezRHIRenderTargetViewHandle GetRenderTarget(ezUInt8 uiIndex) const;
  inline ezRHIRenderTargetViewHandle GetDepthStencilTarget() const;

  void DestroyAllAttachedViews();

protected:
  ezRHIRenderTargetViewHandle m_hRTs[EZ_GAL_MAX_RENDERTARGET_COUNT];
  ezRHIRenderTargetViewHandle m_hDSTarget;

  ezUInt8 m_uiMaxRTIndex;
};

struct ezRHIRenderingSetup
{
  ezRHIRenderTargetSetup m_RenderTargetSetup;
  ezColor m_ClearColor = ezColor(0, 0, 0, 0);
  ezUInt32 m_uiRenderTargetClearMask = 0x0;
  float m_fDepthClear = 1.0f;
  ezUInt8 m_uiStencilClear = 0;
  bool m_bClearDepth = false;
  bool m_bClearStencil = false;
  bool m_bDiscardColor = false;
  bool m_bDiscardDepth = false;
};

#include <RHI/Renderer/Resources/Implementation/RenderTargetSetup_inl.h>
