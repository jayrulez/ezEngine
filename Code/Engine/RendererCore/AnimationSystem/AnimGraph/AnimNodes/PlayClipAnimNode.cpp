#include <RendererCorePCH.h>

#include <RendererCore/AnimationSystem/AnimGraph/AnimGraph.h>
#include <RendererCore/AnimationSystem/AnimGraph/AnimNodes/PlayClipAnimNode.h>
#include <RendererCore/AnimationSystem/SkeletonResource.h>

// clang-format off
EZ_BEGIN_DYNAMIC_REFLECTED_TYPE(ezPlayClipAnimNode, 1, ezRTTIDefaultAllocator<ezPlayClipAnimNode>)
  {
    EZ_BEGIN_PROPERTIES
    {
      EZ_ARRAY_ACCESSOR_PROPERTY("Clips", Clips_GetCount, Clips_GetValue, Clips_SetValue, Clips_Insert, Clips_Remove)->AddAttributes(new ezAssetBrowserAttribute("Animation Clip")),
      EZ_MEMBER_PROPERTY("PlaybackSpeed", m_fPlaybackSpeed)->AddAttributes(new ezDefaultValueAttribute(1.0f)),
      EZ_MEMBER_PROPERTY("AnimRamp", m_AnimRamp),
      EZ_MEMBER_PROPERTY("ApplyRootMotion", m_bApplyRootMotion),
      EZ_MEMBER_PROPERTY("Loop", m_bLoop),
      EZ_MEMBER_PROPERTY("CancelWhenInactive", m_bCancelWhenInactive),

      EZ_MEMBER_PROPERTY("Active", m_ActivePin)->AddAttributes(new ezHiddenAttribute()),
      EZ_MEMBER_PROPERTY("Weights", m_WeightsPin)->AddAttributes(new ezHiddenAttribute()),
      EZ_MEMBER_PROPERTY("Speed", m_SpeedPin)->AddAttributes(new ezHiddenAttribute()),
      EZ_MEMBER_PROPERTY("ClipIndex", m_ClipIndexPin)->AddAttributes(new ezHiddenAttribute()),

      EZ_MEMBER_PROPERTY("LocalPose", m_LocalPosePin)->AddAttributes(new ezHiddenAttribute()),
      EZ_MEMBER_PROPERTY("OnFinished", m_OnFinishedPin)->AddAttributes(new ezHiddenAttribute()),
    }
    EZ_END_PROPERTIES;
    EZ_BEGIN_ATTRIBUTES
    {
      new ezCategoryAttribute("Animation Sampling"),
      new ezColorAttribute(ezColor::SteelBlue),
      new ezTitleAttribute("Play: '{Clips[0]}' '{Clips[1]}' '{Clips[2]}'"),
    }
    EZ_END_ATTRIBUTES;
  }
EZ_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

ezResult ezPlayClipAnimNode::SerializeNode(ezStreamWriter& stream) const
{
  stream.WriteVersion(1);

  EZ_SUCCEED_OR_RETURN(SUPER::SerializeNode(stream));

  EZ_SUCCEED_OR_RETURN(m_AnimRamp.Serialize(stream));
  EZ_SUCCEED_OR_RETURN(stream.WriteArray(m_Clips));
  stream << m_fPlaybackSpeed;
  stream << m_bApplyRootMotion;
  stream << m_bLoop;
  stream << m_bCancelWhenInactive;

  EZ_SUCCEED_OR_RETURN(m_ActivePin.Serialize(stream));
  EZ_SUCCEED_OR_RETURN(m_SpeedPin.Serialize(stream));
  EZ_SUCCEED_OR_RETURN(m_ClipIndexPin.Serialize(stream));
  EZ_SUCCEED_OR_RETURN(m_WeightsPin.Serialize(stream));
  EZ_SUCCEED_OR_RETURN(m_LocalPosePin.Serialize(stream));
  EZ_SUCCEED_OR_RETURN(m_OnFinishedPin.Serialize(stream));

  return EZ_SUCCESS;
}

ezResult ezPlayClipAnimNode::DeserializeNode(ezStreamReader& stream)
{
  const auto version = stream.ReadVersion(1);

  EZ_SUCCEED_OR_RETURN(SUPER::DeserializeNode(stream));

  EZ_SUCCEED_OR_RETURN(m_AnimRamp.Deserialize(stream));
  EZ_SUCCEED_OR_RETURN(stream.ReadArray(m_Clips));
  stream >> m_fPlaybackSpeed;
  stream >> m_bApplyRootMotion;
  stream >> m_bLoop;
  stream >> m_bCancelWhenInactive;

  EZ_SUCCEED_OR_RETURN(m_ActivePin.Deserialize(stream));
  EZ_SUCCEED_OR_RETURN(m_SpeedPin.Deserialize(stream));
  EZ_SUCCEED_OR_RETURN(m_ClipIndexPin.Deserialize(stream));
  EZ_SUCCEED_OR_RETURN(m_WeightsPin.Deserialize(stream));
  EZ_SUCCEED_OR_RETURN(m_LocalPosePin.Deserialize(stream));
  EZ_SUCCEED_OR_RETURN(m_OnFinishedPin.Deserialize(stream));

  return EZ_SUCCESS;
}

void ezPlayClipAnimNode::Step(ezAnimGraph& graph, ezTime tDiff, const ezSkeletonResource* pSkeleton, ezGameObject* pTarget)
{
  if (m_Clips.IsEmpty() || !m_LocalPosePin.IsConnected() || m_State.WillStateBeOff(m_ActivePin.IsTriggered(graph)))
  {
    m_uiClipToPlay = 0xFF;
    m_uiNextClipToPlay = 0xFF;
    return;
  }

  ezUInt8 uiNextClip = static_cast<ezUInt8>(m_ClipIndexPin.GetNumber(graph, m_uiNextClipToPlay));

  if (uiNextClip >= m_Clips.GetCount())
  {
    uiNextClip = pTarget->GetWorld()->GetRandomNumberGenerator().UIntInRange(m_Clips.GetCount());
  }

  if (m_uiNextClipToPlay != uiNextClip)
  {
    ezResourceLock<ezAnimationClipResource> pNextClip(m_Clips[uiNextClip], ezResourceAcquireMode::BlockTillLoaded);
    if (pNextClip.GetAcquireResult() != ezResourceAcquireResult::Final)
      return;

    m_uiNextClipToPlay = uiNextClip;
    m_NextClipDuration = pNextClip->GetDescriptor().GetDuration();
  }

  if (m_uiClipToPlay >= m_Clips.GetCount())
  {
    m_uiClipToPlay = uiNextClip;
    m_uiNextClipToPlay = 0xFF; // make sure the next update will pick another random clip
  }

  ezResourceLock<ezAnimationClipResource> pAnimClip(m_Clips[m_uiClipToPlay], ezResourceAcquireMode::BlockTillLoaded);
  if (pAnimClip.GetAcquireResult() != ezResourceAcquireResult::Final)
    return;

  m_State.m_AnimRamp = m_AnimRamp;
  m_State.m_bImmediateRampDown = m_bCancelWhenInactive;
  m_State.m_bImmediateRampUp = false;
  m_State.m_bLoop = m_bLoop;
  m_State.m_bTriggerActive = m_ActivePin.IsTriggered(graph);
  m_State.m_Duration = pAnimClip->GetDescriptor().GetDuration();
  m_State.m_DurationOfQueued = m_NextClipDuration;
  m_State.m_fPlaybackSpeed = m_fPlaybackSpeed * static_cast<float>(m_SpeedPin.GetNumber(graph, 1.0));

  m_State.UpdateState(tDiff);

  if (m_Clips.GetCount() > 1 && m_State.HasTransitioned())
  {
    m_uiClipToPlay = uiNextClip; // don't use m_uiNextClipToPlay here, it can be 0xFF
    m_uiNextClipToPlay = 0xFF;
    m_NextClipDuration.SetZero();
  }

  if (m_State.GetCurrentState() == ezAnimState::State::StartedRampDown)
  {
    m_OnFinishedPin.SetTriggered(graph, true);
  }

  void* pThis = this;
  auto& cmd = graph.GetPoseGenerator().AllocCommandSampleTrack(ezHashingUtils::xxHash32(&pThis, sizeof(pThis)));
  cmd.m_hAnimationClip = m_Clips[m_uiClipToPlay];
  cmd.m_fNormalizedSamplePos = m_State.GetNormalizedPlaybackPosition();

  {
    ezAnimGraphPinDataLocalTransforms* pLocalTransforms = graph.AddPinDataLocalTransforms();

    pLocalTransforms->m_fOverallWeight = m_State.GetWeight();
    pLocalTransforms->m_pWeights = m_WeightsPin.GetWeights(graph);

    if (m_bApplyRootMotion)
    {
      pLocalTransforms->m_bUseRootMotion = true;

      pLocalTransforms->m_vRootMotion = pAnimClip->GetDescriptor().m_vConstantRootMotion * tDiff.AsFloatInSeconds() * m_State.m_fPlaybackSpeed;
    }

    pLocalTransforms->m_CommandID = cmd.GetCommandID();

    m_LocalPosePin.SetPose(graph, pLocalTransforms);
  }
}

ezUInt32 ezPlayClipAnimNode::Clips_GetCount() const
{
  return m_Clips.GetCount();
}

const char* ezPlayClipAnimNode::Clips_GetValue(ezUInt32 uiIndex) const
{
  const auto& hMat = m_Clips[uiIndex];

  if (!hMat.IsValid())
    return "";

  return hMat.GetResourceID();
}

void ezPlayClipAnimNode::Clips_SetValue(ezUInt32 uiIndex, const char* value)
{
  if (ezStringUtils::IsNullOrEmpty(value))
    m_Clips[uiIndex] = ezAnimationClipResourceHandle();
  else
  {
    m_Clips[uiIndex] = ezResourceManager::LoadResource<ezAnimationClipResource>(value);
  }
}

void ezPlayClipAnimNode::Clips_Insert(ezUInt32 uiIndex, const char* value)
{
  ezAnimationClipResourceHandle hMat;

  if (!ezStringUtils::IsNullOrEmpty(value))
    hMat = ezResourceManager::LoadResource<ezAnimationClipResource>(value);

  m_Clips.Insert(hMat, uiIndex);
}

void ezPlayClipAnimNode::Clips_Remove(ezUInt32 uiIndex)
{
  m_Clips.RemoveAtAndCopy(uiIndex);
}
