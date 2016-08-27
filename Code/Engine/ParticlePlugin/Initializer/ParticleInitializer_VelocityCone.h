#pragma once

#include <ParticlePlugin/Initializer/ParticleInitializer.h>

class ezParticleInitializerFactory_VelocityCone : public ezParticleInitializerFactory
{
  EZ_ADD_DYNAMIC_REFLECTION(ezParticleInitializerFactory_VelocityCone, ezParticleInitializerFactory);

public:
  ezParticleInitializerFactory_VelocityCone();

  virtual const ezRTTI* GetInitializerType() const override;
  virtual void CopyInitializerProperties(ezParticleInitializer* pInitializer) const override;

  virtual void Save(ezStreamWriter& stream) const override;
  virtual void Load(ezStreamReader& stream) override;

public:
  ezAngle m_Angle;
  float m_fMinSpeed;
  float m_fSpeedRange;
};


class EZ_PARTICLEPLUGIN_DLL ezParticleInitializer_VelocityCone : public ezParticleInitializer
{
  EZ_ADD_DYNAMIC_REFLECTION(ezParticleInitializer_VelocityCone, ezParticleInitializer);

public:

  ezAngle m_Angle;
  float m_fMinSpeed;
  float m_fSpeedRange;


  virtual void CreateRequiredStreams() override;

protected:
  virtual void SpawnElements(ezUInt64 uiStartIndex, ezUInt64 uiNumElements) override;

  ezStream* m_pStreamVelocity;
};
