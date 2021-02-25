#include <GameEnginePCH.h>

#include <Core/WorldSerializer/WorldReader.h>
#include <Core/WorldSerializer/WorldWriter.h>
#include <GameEngine/Animation/Skeletal/AnimatedMeshComponent.h>
#include <RendererCore/AnimationSystem/SkeletonResource.h>
#include <RendererFoundation/Device/Device.h>

#include <RendererCore/AnimationSystem/Declarations.h>
#include <RendererCore/Debug/DebugRenderer.h>
#include <ozz/animation/runtime/local_to_model_job.h>
#include <ozz/animation/runtime/skeleton.h>
#include <ozz/base/containers/vector.h>
#include <ozz/base/maths/simd_math.h>
#include <ozz/base/maths/soa_transform.h>
#include <ozz/base/span.h>

// clang-format off
EZ_BEGIN_COMPONENT_TYPE(ezAnimatedMeshComponent, 13, ezComponentMode::Dynamic); // TODO: why dynamic ? (I guess because the overridden CreateRenderData() has to be called every frame)
{
  EZ_BEGIN_ATTRIBUTES
  {
      new ezCategoryAttribute("Animation"),
  }
  EZ_END_ATTRIBUTES;

  EZ_BEGIN_MESSAGEHANDLERS
  {
    EZ_MESSAGE_HANDLER(ezMsgAnimationPoseUpdated, OnAnimationPoseUpdated),
    EZ_MESSAGE_HANDLER(ezMsgQueryAnimationSkeleton, OnQueryAnimationSkeleton),
  }
  EZ_END_MESSAGEHANDLERS;
}
EZ_END_COMPONENT_TYPE
// clang-format on

ezAnimatedMeshComponent::ezAnimatedMeshComponent() = default;
ezAnimatedMeshComponent::~ezAnimatedMeshComponent() = default;

void ezAnimatedMeshComponent::SerializeComponent(ezWorldWriter& stream) const
{
  SUPER::SerializeComponent(stream);
  auto& s = stream.GetStream();
}

void ezAnimatedMeshComponent::DeserializeComponent(ezWorldReader& stream)
{
  SUPER::DeserializeComponent(stream);
  const ezUInt32 uiVersion = stream.GetComponentTypeVersion(GetStaticRTTI());
  auto& s = stream.GetStream();

  EZ_ASSERT_DEV(uiVersion >= 13, "Unsupported version, delete the file and reexport it");
}

void ezAnimatedMeshComponent::OnActivated()
{
  SUPER::OnActivated();

  // make sure the skinning buffer is deleted
  EZ_ASSERT_DEBUG(m_hSkinningTransformsBuffer.IsInvalidated(), "The skinning buffer should not exist at this time");

  InitializeAnimationPose();
}

void ezAnimatedMeshComponent::OnDeactivated()
{
  m_SkinningSpacePose.Clear();

  SUPER::OnDeactivated();
}

void ezAnimatedMeshComponent::InitializeAnimationPose()
{
  if (!m_hMesh.IsValid())
    return;

  ezResourceLock<ezMeshResource> pMesh(m_hMesh, ezResourceAcquireMode::BlockTillLoaded);
  if (pMesh.GetAcquireResult() != ezResourceAcquireResult::Final)
    return;

  const auto hSkeleton = pMesh->m_hDefaultSkeleton;

  if (!hSkeleton.IsValid())
    return;

  ezResourceLock<ezSkeletonResource> pSkeleton(hSkeleton, ezResourceAcquireMode::BlockTillLoaded);
  if (pSkeleton.GetAcquireResult() != ezResourceAcquireResult::Final)
    return;

  {
    const ozz::animation::Skeleton* pOzzSkeleton = &pSkeleton->GetDescriptor().m_Skeleton.GetOzzSkeleton();
    const ezUInt32 uiNumSkeletonJoints = pOzzSkeleton->num_joints();

    ezArrayPtr<ezMat4> pPoseMatrices = EZ_NEW_ARRAY(ezFrameAllocator::GetCurrentAllocator(), ezMat4, uiNumSkeletonJoints);

    {
      ozz::animation::LocalToModelJob job;
      job.input = pOzzSkeleton->joint_bind_poses();
      job.output = ozz::span<ozz::math::Float4x4>(reinterpret_cast<ozz::math::Float4x4*>(pPoseMatrices.GetPtr()), reinterpret_cast<ozz::math::Float4x4*>(pPoseMatrices.GetEndPtr()));
      job.skeleton = pOzzSkeleton;
      job.Run();
    }

    ezMsgAnimationPoseUpdated msg;
    msg.m_ModelTransforms = pPoseMatrices;
    msg.m_pRootTransform = &pSkeleton->GetDescriptor().m_RootTransform;
    msg.m_pSkeleton = &pSkeleton->GetDescriptor().m_Skeleton;

    OnAnimationPoseUpdated(msg);
  }

  // Create the buffer for the skinning matrices
  CreateSkinningTransformBuffer(m_SkinningSpacePose.m_Transforms);
}

ezMeshRenderData* ezAnimatedMeshComponent::CreateRenderData() const
{
  if (!m_SkinningSpacePose.IsEmpty())
  {
    // this copy is necessary for the multi-threaded renderer to not access m_SkinningSpacePose while we update it
    ezArrayPtr<ezMat4> pRenderMatrices = EZ_NEW_ARRAY(ezFrameAllocator::GetCurrentAllocator(), ezMat4, m_SkinningSpacePose.GetTransformCount());
    pRenderMatrices.CopyFrom(m_SkinningSpacePose.m_Transforms);

    m_SkinningMatrices = pRenderMatrices;
  }

  ezMeshRenderData* pData = SUPER::CreateRenderData();
  pData->m_GlobalTransform = m_RootTransform;

  return pData;
}

void ezAnimatedMeshComponent::OnAnimationPoseUpdated(ezMsgAnimationPoseUpdated& msg)
{
  if (!m_hMesh.IsValid())
    return;

  m_RootTransform = *msg.m_pRootTransform;

  ezResourceLock<ezMeshResource> pMesh(m_hMesh, ezResourceAcquireMode::BlockTillLoaded);

  m_SkinningSpacePose.MapModelSpacePoseToSkinningSpace(pMesh->m_Bones, *msg.m_pSkeleton, msg.m_ModelTransforms);
}

void ezAnimatedMeshComponent::OnQueryAnimationSkeleton(ezMsgQueryAnimationSkeleton& msg)
{
  if (!msg.m_hSkeleton.IsValid())
  {
    // only overwrite, if no one else had a better skeleton (e.g. the ezSkeletonComponent)

    ezResourceLock<ezMeshResource> pMesh(m_hMesh, ezResourceAcquireMode::BlockTillLoaded);
    if (pMesh.GetAcquireResult() == ezResourceAcquireResult::Final)
    {
      msg.m_hSkeleton = pMesh->m_hDefaultSkeleton;
    }
  }
}

ezResult ezAnimatedMeshComponent::GetLocalBounds(ezBoundingBoxSphere& bounds, bool& bAlwaysVisible)
{
  if (m_hMesh.IsValid())
  {
    ezResourceLock<ezMeshResource> pMesh(m_hMesh, ezResourceAcquireMode::AllowLoadingFallback);
    bounds = pMesh->GetBounds();
    bounds.Transform(m_RootTransform.GetAsMat4());
    return EZ_SUCCESS;
  }

  return EZ_FAILURE;
}

EZ_STATICLINK_FILE(GameEngine, GameEngine_Animation_Skeletal_Implementation_AnimatedMeshComponent);
