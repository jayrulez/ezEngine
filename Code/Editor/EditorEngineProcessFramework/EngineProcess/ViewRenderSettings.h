#pragma once

#include <Core/Graphics/Camera.h>
#include <EditorEngineProcessFramework/EditorEngineProcessFrameworkDLL.h>
#include <EditorEngineProcessFramework/IPC/SyncObject.h>
#include <Foundation/Reflection/Reflection.h>
#include <RendererCore/Pipeline/ViewRenderMode.h>

struct EZ_EDITORENGINEPROCESSFRAMEWORK_DLL ezSceneViewPerspective
{
  typedef ezUInt8 StorageType;

  enum Enum
  {
    Orthogonal_Front,
    Orthogonal_Right,
    Orthogonal_Top,
    Perspective,

    Default = Perspective
  };
};
EZ_DECLARE_REFLECTABLE_TYPE(EZ_EDITORENGINEPROCESSFRAMEWORK_DLL, ezSceneViewPerspective);

struct EZ_EDITORENGINEPROCESSFRAMEWORK_DLL ezEngineViewConfig
{
  ezEngineViewConfig()
  {
    m_RenderMode = ezViewRenderMode::Default;
    m_Perspective = ezSceneViewPerspective::Default;
    m_CameraUsageHint = ezCameraUsageHint::EditorView;
    m_pLinkedViewConfig = nullptr;
  }

  ezViewRenderMode::Enum m_RenderMode;
  ezSceneViewPerspective::Enum m_Perspective;
  ezCameraUsageHint::Enum m_CameraUsageHint;
  bool m_bUseCameraTransformOnDevice = true;

  ezCamera m_Camera;
  ezEngineViewConfig* m_pLinkedViewConfig; // used to store which other view config this is linked to, for resetting values when switching views

  void ApplyPerspectiveSetting(float fov = 0.0f, float nearPlane = 0.1f, float farPlane = 1000.0f);
};
struct EZ_EDITORENGINEPROCESSFRAMEWORK_DLL ezEngineViewLightSettingsEvent
{
  enum class Type
  {
    SkyBoxChanged,
    SkyLightChanged,
    SkyLightCubeMapChanged,
    SkyLightIntensityChanged,
    DirectionalLightChanged,
    DirectionalLightAngleChanged,
    DirectionalLightShadowsChanged,
    DirectionalLightIntensityChanged,
    FogChanged,
    DefaultValuesChanged,
  };

  Type m_Type;
};

class EZ_EDITORENGINEPROCESSFRAMEWORK_DLL ezEngineViewLightSettings : public ezEditorEngineSyncObject
{
  EZ_ADD_DYNAMIC_REFLECTION(ezEngineViewLightSettings, ezEditorEngineSyncObject);

public:
  ezEngineViewLightSettings(bool bEnable = true);
  ~ezEngineViewLightSettings();

  bool GetSkyBox() const;
  void SetSkyBox(bool val);

  bool GetSkyLight() const;
  void SetSkyLight(bool val);

  const char* GetSkyLightCubeMap() const;
  void SetSkyLightCubeMap(const char* val);

  float GetSkyLightIntensity() const;
  void SetSkyLightIntensity(float val);

  bool GetDirectionalLight() const;
  void SetDirectionalLight(bool val);

  ezAngle GetDirectionalLightAngle() const;
  void SetDirectionalLightAngle(ezAngle val);

  bool GetDirectionalLightShadows() const;
  void SetDirectionalLightShadows(bool val);

  float GetDirectionalLightIntensity() const;
  void SetDirectionalLightIntensity(float val);

  bool GetFog() const;
  void SetFog(bool val);

  mutable ezEvent<const ezEngineViewLightSettingsEvent&> m_EngineViewLightSettingsEvents;

  virtual bool SetupForEngine(ezWorld* pWorld, ezUInt32 uiNextComponentPickingID);
  virtual void UpdateForEngine(ezWorld* pWorld);

private:
  void SetModifiedInternal(ezEngineViewLightSettingsEvent::Type type);

  bool m_bSkyBox = true;
  bool m_bSkyLight = true;
  ezString m_sSkyLightCubeMap = "{ 0b202e08-a64f-465d-b38e-15b81d161822 }";
  float m_fSkyLightIntensity = 1.0f;

  bool m_bDirectionalLight = true;
  ezAngle m_DirectionalLightAngle = ezAngle::Degree(30.0f);
  bool m_bDirectionalLightShadows = false;
  float m_fDirectionalLightIntensity = 10.0f;

  bool m_bFog = false;

  // Engine side data
  ezWorld* m_pWorld = nullptr;
  ezGameObjectHandle m_hSkyBoxObject;
  ezComponentHandle m_hSkyBox;
  ezGameObjectHandle m_hGameObject;
  ezComponentHandle m_hDirLight;
  ezComponentHandle m_hSkyLight;
  ezComponentHandle m_hFog;
};
