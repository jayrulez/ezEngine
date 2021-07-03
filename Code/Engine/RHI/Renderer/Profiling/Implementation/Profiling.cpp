#include <RHIPCH.h>

#include <Foundation/Configuration/Startup.h>
#include <Foundation/Profiling/Profiling.h>
#include <RHI/Renderer/CommandEncoder/CommandEncoder.h>
#include <RHI/Renderer/Device/Device.h>
#include <RHI/Renderer/Profiling/Profiling.h>

#if EZ_ENABLED(EZ_USE_PROFILING)

struct GPUTimingScope
{
  EZ_DECLARE_POD_TYPE();

  ezRHITimestampHandle m_BeginTimestamp;
  ezRHITimestampHandle m_EndTimestamp;
  char m_szName[48];
};

class GPUProfilingSystem
{
public:
  static void ProcessTimestamps(const ezRHIDeviceEvent& e)
  {
    if (e.m_Type != ezRHIDeviceEvent::AfterEndFrame)
      return;

    while (!m_TimingScopes.IsEmpty())
    {
      auto& timingScope = m_TimingScopes.PeekFront();

      ezTime endTime;
      if (e.m_pDevice->GetTimestampResult(timingScope.m_EndTimestamp, endTime).Succeeded())
      {
        ezTime beginTime;
        EZ_VERIFY(e.m_pDevice->GetTimestampResult(timingScope.m_BeginTimestamp, beginTime).Succeeded(),
          "Begin timestamp should be finished before end timestamp");

        if (!beginTime.IsZero() && !endTime.IsZero())
        {
          ezProfilingSystem::AddGPUScope(timingScope.m_szName, beginTime, endTime);
        }

        m_TimingScopes.PopFront();
      }
      else
      {
        // Timestamps are not available yet
        break;
      }
    }
  }

  static GPUTimingScope& AllocateScope() { return m_TimingScopes.ExpandAndGetRef(); }

private:
  static void OnEngineStartup() { ezRHIDevice::GetDefaultDevice()->m_Events.AddEventHandler(&GPUProfilingSystem::ProcessTimestamps); }

  static void OnEngineShutdown() { ezRHIDevice::GetDefaultDevice()->m_Events.RemoveEventHandler(&GPUProfilingSystem::ProcessTimestamps); }

  static ezDeque<GPUTimingScope, ezStaticAllocatorWrapper> m_TimingScopes;

  EZ_MAKE_SUBSYSTEM_STARTUP_FRIEND(RendererFoundation, GPUProfilingSystem);
};

// clang-format off
EZ_BEGIN_SUBSYSTEM_DECLARATION(RendererFoundation, GPUProfilingSystem)

  BEGIN_SUBSYSTEM_DEPENDENCIES
    "Foundation",
    "Core"
  END_SUBSYSTEM_DEPENDENCIES

  ON_HIGHLEVELSYSTEMS_STARTUP
  {
    GPUProfilingSystem::OnEngineStartup();
  }

  ON_HIGHLEVELSYSTEMS_SHUTDOWN
  {
    GPUProfilingSystem::OnEngineShutdown();
  }

EZ_END_SUBSYSTEM_DECLARATION;
// clang-format on

ezDeque<GPUTimingScope, ezStaticAllocatorWrapper> GPUProfilingSystem::m_TimingScopes;

//////////////////////////////////////////////////////////////////////////

ezProfilingScopeAndMarker::ezProfilingScopeAndMarker(ezRHICommandEncoder* pCommandEncoder, const char* szName)
  : ezProfilingScope(szName, nullptr)
  , m_pCommandEncoder(pCommandEncoder)
{
  pCommandEncoder->PushMarker(m_szName);

  auto& timingScope = GPUProfilingSystem::AllocateScope();
  timingScope.m_BeginTimestamp = pCommandEncoder->InsertTimestamp();
  ezStringUtils::Copy(timingScope.m_szName, EZ_ARRAY_SIZE(timingScope.m_szName), m_szName);

  m_pTimingScope = &timingScope;
}

ezProfilingScopeAndMarker::~ezProfilingScopeAndMarker()
{
  m_pCommandEncoder->PopMarker();
  m_pTimingScope->m_EndTimestamp = m_pCommandEncoder->InsertTimestamp();
}

#endif

EZ_STATICLINK_FILE(RendererFoundation, RendererFoundation_Profiling_Implementation_Profiling);
