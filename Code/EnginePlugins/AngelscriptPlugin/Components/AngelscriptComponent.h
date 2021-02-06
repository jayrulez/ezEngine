#pragma once

#include <AngelscriptPlugin/AngelscriptManager.h>

#include <Core/ResourceManager/ResourceHandle.h>
#include <Core/World/Component.h>
#include <Core/World/EventMessageHandlerComponent.h>
#include <Core/World/World.h>
#include <Foundation/Containers/ArrayMap.h>
#include <Foundation/Types/RangeView.h>

class ezAngelscriptComponent;

struct EZ_ANGELSCRIPTPLUGIN_DLL ezMsgAngelscriptMsgProxy : public ezMessage
{
  EZ_DECLARE_MESSAGE_TYPE(ezMsgAngelscriptMsgProxy, ezMessage);

  ezUInt32 m_uiTypeNameHash = 0;
  ezUInt32 m_uiStashIndex = 0;
};

class EZ_ANGELSCRIPTPLUGIN_DLL ezAngelscriptComponentManager : public ezComponentManager<class ezAngelscriptComponent, ezBlockStorageType::FreeList>
{
  using SUPER = ezComponentManager<class ezAngelscriptComponent, ezBlockStorageType::FreeList>;

public:
  ezAngelscriptComponentManager(ezWorld* pWorld);
  ~ezAngelscriptComponentManager();

  virtual void Initialize() override;
  virtual void Deinitialize() override;
  virtual void OnSimulationStarted() override;

private:
  void Update(const ezWorldModule::UpdateContext& context);
};

//////////////////////////////////////////////////////////////////////////

class EZ_ANGELSCRIPTPLUGIN_DLL ezAngelscriptComponent : public ezEventMessageHandlerComponent
{
  EZ_DECLARE_COMPONENT_TYPE(ezAngelscriptComponent, ezEventMessageHandlerComponent, ezAngelscriptComponentManager);

  //////////////////////////////////////////////////////////////////////////
  // ezComponent

protected:
  virtual void SerializeComponent(ezWorldWriter& stream) const override;
  virtual void DeserializeComponent(ezWorldReader& stream) override;
  virtual void Initialize() override;
  virtual void Deinitialize() override;
  virtual void OnActivated() override;
  virtual void OnDeactivated() override;
  virtual void OnSimulationStarted() override;

  virtual bool OnUnhandledMessage(ezMessage& msg, bool bWasPostedMsg) override;
  virtual bool OnUnhandledMessage(ezMessage& msg, bool bWasPostedMsg) const override;

  bool HandleUnhandledMessage(ezMessage& msg, bool bWasPostedMsg);

  //////////////////////////////////////////////////////////////////////////
  // ezEventMessageHandlerComponent

protected:
  virtual bool HandlesEventMessage(const ezEventMessage& msg) const override;

  //////////////////////////////////////////////////////////////////////////
  // ezAngelscriptComponent

public:
  ezAngelscriptComponent();
  ~ezAngelscriptComponent();

  void BroadcastEventMsg(ezEventMessage& msg);

  void SetUpdateInterval(ezTime interval) { m_UpdateInterval = interval; }

  void SetAngelscriptComponentGuid(const ezUuid& hResource);
  const ezUuid& GetAngelscriptComponentGuid() const;

private:
  struct EventSender
  {
    const ezRTTI* m_pMsgType = nullptr;
    ezEventMessageSender<ezEventMessage> m_Sender;
  };

  ezHybridArray<EventSender, 2> m_EventSenders;

  void Update();
  void SetAngelscriptComponentClassName(const char* szClassName); // [ property ]
  const char* GetAngelscriptComponentClassName() const;           // [ property ]
  void OnMsgAngelscriptMsgProxy(ezMsgAngelscriptMsgProxy& msg);   // [ message handler ]

private:
  ezUuid m_AngelscriptComponentGuid;
  ezTime m_LastUpdate;
  ezTime m_UpdateInterval = ezTime::Seconds(-1); // deactivated by default
};
