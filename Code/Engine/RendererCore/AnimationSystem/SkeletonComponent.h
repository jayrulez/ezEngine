#pragma once

#include <Foundation/Math/Declarations.h>
#include <RendererCore/AnimationSystem/SkeletonResource.h>
#include <RendererCore/Components/RenderComponent.h>
#include <RendererCore/Debug/DebugRenderer.h>

struct ezMsgQueryAnimationSkeleton;

using ezVisualizeSkeletonComponentManager = ezComponentManagerSimple<class ezSkeletonComponent, ezComponentUpdateType::WhenSimulating, ezBlockStorageType::Compact>;

class EZ_RENDERERCORE_DLL ezSkeletonComponent : public ezRenderComponent
{
  EZ_DECLARE_COMPONENT_TYPE(ezSkeletonComponent, ezRenderComponent, ezVisualizeSkeletonComponentManager);

  //////////////////////////////////////////////////////////////////////////
  // ezComponent

public:
  virtual void SerializeComponent(ezWorldWriter& stream) const override;
  virtual void DeserializeComponent(ezWorldReader& stream) override;

protected:
  virtual void OnSimulationStarted() override;


  //////////////////////////////////////////////////////////////////////////
  // ezRenderComponent

public:
  virtual ezResult GetLocalBounds(ezBoundingBoxSphere& bounds, bool& bAlwaysVisible) override;

  //////////////////////////////////////////////////////////////////////////
  // ezSkeletonComponent

public:
  ezSkeletonComponent();
  ~ezSkeletonComponent();

  void SetSkeletonFile(const char* szFile); // [ property ]
  const char* GetSkeletonFile() const;      // [ property ]

  void SetSkeleton(const ezSkeletonResourceHandle& hResource);
  const ezSkeletonResourceHandle& GetSkeleton() const { return m_hSkeleton; }

  bool m_bVisualizeSkeleton = false; // [ property ]

protected:
  void Update();
  void OnAnimationPoseUpdated(ezMsgAnimationPoseUpdated& msg); // [ msg handler ]
  void OnQueryAnimationSkeleton(ezMsgQueryAnimationSkeleton& msg);
  void UpdateSkeletonVis();

  ezSkeletonResourceHandle m_hSkeleton;
  ezBoundingBoxSphere m_LocalBounds;
  ezDynamicArray<ezDebugRenderer::Line> m_LinesSkeleton;
};