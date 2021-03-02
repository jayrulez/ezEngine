#pragma once

#include <EditorEngineProcessFramework/EngineProcess/EngineProcessViewContext.h>

class ezAnimationClipContext;

class ezAnimationClipViewContext : public ezEngineProcessViewContext
{
public:
  ezAnimationClipViewContext(ezAnimationClipContext* pContext);
  ~ezAnimationClipViewContext();

  bool UpdateThumbnailCamera(const ezBoundingBoxSphere& bounds);

protected:
  virtual ezViewHandle CreateView() override;
  virtual void SetCamera(const ezViewRedrawMsgToEngine* pMsg) override;

  ezAnimationClipContext* m_pContext = nullptr;
};
