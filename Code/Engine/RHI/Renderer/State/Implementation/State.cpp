#include <RHIPCH.h>

#include <RHI/Renderer/Basics.h>
#include <RHI/Renderer/State/State.h>

ezRHIBlendState::ezRHIBlendState(const ezRHIBlendStateCreationDescription& Description)
  : ezRHIObject(Description)
{
}

ezRHIBlendState::~ezRHIBlendState() {}



ezRHIDepthStencilState::ezRHIDepthStencilState(const ezRHIDepthStencilStateCreationDescription& Description)
  : ezRHIObject(Description)
{
}

ezRHIDepthStencilState::~ezRHIDepthStencilState() {}



ezRHIRasterizerState::ezRHIRasterizerState(const ezRHIRasterizerStateCreationDescription& Description)
  : ezRHIObject(Description)
{
}

ezRHIRasterizerState::~ezRHIRasterizerState() {}


ezRHISamplerState::ezRHISamplerState(const ezRHISamplerStateCreationDescription& Description)
  : ezRHIObject(Description)
{
}

ezRHISamplerState::~ezRHISamplerState() {}



EZ_STATICLINK_FILE(RendererFoundation, RendererFoundation_State_Implementation_State);
