#pragma once

#include <ParticlePlugin/Basics.h>
#include <ParticlePlugin/Effect/ParticleEffectInstance.h>

class ezParticleWorldModule;
class ezParticleEffectInstance;
typedef ezTypedResourceHandle<class ezParticleEffectResource> ezParticleEffectResourceHandle;

class EZ_PARTICLEPLUGIN_DLL ezParticleEffectController
{
public:
  ezParticleEffectController();
  ezParticleEffectController(const ezParticleEffectController& rhs);
  void operator=(const ezParticleEffectController& rhs);

  void Create(const ezParticleEffectResourceHandle& hEffectResource, ezParticleWorldModule* pModule, ezUInt64 uiRandomSeed);

  bool IsValid() const;
  void Invalidate();

  bool IsAlive() const;
  void SetTransform(const ezTransform& t) const;

  void Tick(const ezTime& tDiff) const;

  void StopImmediate();

private:
  ezParticleEffectController(ezParticleWorldModule* pModule, ezParticleEffectHandle hEffect);
  ezParticleEffectInstance* GetInstance() const;

  ezParticleWorldModule* m_pModule;
  ezParticleEffectHandle m_hEffect;
};