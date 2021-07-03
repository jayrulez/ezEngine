#include <RHIPCH.h>

#include <RHI/Renderer/Resources/Query.h>

ezRHIQuery::ezRHIQuery(const ezRHIQueryCreationDescription& Description)
  : ezRHIResource<ezRHIQueryCreationDescription>(Description)
  , m_bStarted(false)
{
}

ezRHIQuery::~ezRHIQuery() {}

EZ_STATICLINK_FILE(RendererFoundation, RendererFoundation_Resources_Implementation_Query);
