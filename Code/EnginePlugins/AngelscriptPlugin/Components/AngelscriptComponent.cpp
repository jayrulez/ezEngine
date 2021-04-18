#include <AngelscriptPluginPCH.h>

#include <Core/Messages/EventMessage.h>
#include <Core/ResourceManager/ResourceManager.h>
#include <Core/WorldSerializer/WorldReader.h>
#include <Core/WorldSerializer/WorldWriter.h>
#include <Foundation/IO/FileSystem/FileReader.h>
#include <Foundation/IO/FileSystem/FileWriter.h>
#include <AngelscriptPlugin/Components/AngelscriptComponent.h>

// clang-format off
EZ_IMPLEMENT_MESSAGE_TYPE(ezMsgAngelscriptMsgProxy);
EZ_BEGIN_DYNAMIC_REFLECTED_TYPE(ezMsgAngelscriptMsgProxy, 1, ezRTTIDefaultAllocator<ezMsgAngelscriptMsgProxy>)
EZ_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

// clang-format off
EZ_BEGIN_COMPONENT_TYPE(ezAngelscriptComponent, 4, ezComponentMode::Static)
{
  EZ_BEGIN_PROPERTIES
  {
    EZ_ACCESSOR_PROPERTY("Script", GetAngelscriptComponentClassName, SetAngelscriptComponentClassName)->AddAttributes(new ezAssetBrowserAttribute("Angelscript")),
  }
  EZ_END_PROPERTIES;
  EZ_BEGIN_ATTRIBUTES
  {
    new ezCategoryAttribute("Scripting"),
  }
  EZ_END_ATTRIBUTES;
  EZ_BEGIN_MESSAGEHANDLERS
  {
    EZ_MESSAGE_HANDLER(ezMsgAngelscriptMsgProxy, OnMsgAngelscriptMsgProxy)
  }
  EZ_END_MESSAGEHANDLERS;
}
EZ_END_COMPONENT_TYPE;
// clang-format on

ezAngelscriptComponent::ezAngelscriptComponent() = default;
ezAngelscriptComponent::~ezAngelscriptComponent() = default;

void ezAngelscriptComponent::SerializeComponent(ezWorldWriter& stream) const
{
  SUPER::SerializeComponent(stream);

  auto& s = stream.GetStream();
}

void ezAngelscriptComponent::DeserializeComponent(ezWorldReader& stream)
{
  const ezUInt32 uiVersion = stream.GetComponentTypeVersion(GetStaticRTTI());

  if (uiVersion >= 4)
  {
    SUPER::DeserializeComponent(stream);
  }

  auto& s = stream.GetStream();
}

bool ezAngelscriptComponent::OnUnhandledMessage(ezMessage& msg, bool bWasPostedMsg)
{
  return HandleUnhandledMessage(msg, bWasPostedMsg);
}

bool ezAngelscriptComponent::OnUnhandledMessage(ezMessage& msg, bool bWasPostedMsg) const
{
  return const_cast<ezAngelscriptComponent*>(this)->HandleUnhandledMessage(msg, bWasPostedMsg);
}

bool ezAngelscriptComponent::HandleUnhandledMessage(ezMessage& msg, bool bWasPostedMsg)
{
  return false;
}

bool ezAngelscriptComponent::HandlesEventMessage(const ezEventMessage& msg) const
{
  return false;
}

void ezAngelscriptComponent::BroadcastEventMsg(ezEventMessage& msg)
{
  const ezRTTI* pType = msg.GetDynamicRTTI();

  for (auto& sender : m_EventSenders)
  {
    if (sender.m_pMsgType == pType)
    {
      sender.m_Sender.SendEventMessage(msg, this, GetOwner()->GetParent());
      return;
    }
  }

  auto& sender = m_EventSenders.ExpandAndGetRef();
  sender.m_pMsgType = pType;
  sender.m_Sender.SendEventMessage(msg, this, GetOwner()->GetParent());
}

void ezAngelscriptComponent::SetAngelscriptComponentGuid(const ezUuid& hResource)
{
}

const ezUuid& ezAngelscriptComponent::GetAngelscriptComponentGuid() const
{
  return m_AngelscriptComponentGuid;
}

void ezAngelscriptComponent::Initialize()
{
}

void ezAngelscriptComponent::Deinitialize()
{

}

void ezAngelscriptComponent::OnActivated()
{

}

void ezAngelscriptComponent::OnDeactivated()
{

}

void ezAngelscriptComponent::OnSimulationStarted()
{

}

void ezAngelscriptComponent::Update()
{
}

void ezAngelscriptComponent::SetAngelscriptComponentClassName(const char* szClassName)
{
}

const char* ezAngelscriptComponent::GetAngelscriptComponentClassName() const
{
  return nullptr;
}

void ezAngelscriptComponent::OnMsgAngelscriptMsgProxy(ezMsgAngelscriptMsgProxy& msg)
{
}
