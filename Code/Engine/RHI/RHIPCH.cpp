#include <RHIPCH.h>

EZ_STATICLINK_LIBRARY(RHI)
{
  if (bReturn)
    return;

  EZ_STATICLINK_REFERENCE(RHI_Framebuffer_FramebufferBase);
  EZ_STATICLINK_REFERENCE(RHI_HighLevelRenderer_BufferLayout);
  EZ_STATICLINK_REFERENCE(RHI_HighLevelRenderer_ObjectCache);
  EZ_STATICLINK_REFERENCE(RHI_HighLevelRenderer_RenderCommandListImpl);
  EZ_STATICLINK_REFERENCE(RHI_HighLevelRenderer_RenderDeviceImpl);
  EZ_STATICLINK_REFERENCE(RHI_Instance_Instance);
  EZ_STATICLINK_REFERENCE(RHI_Program_ProgramBase);
  EZ_STATICLINK_REFERENCE(RHI_Resource_ResourceBase);
  EZ_STATICLINK_REFERENCE(RHI_Resource_ResourceStateTracker);
  EZ_STATICLINK_REFERENCE(RHI_Shader_ShaderBase);
}
