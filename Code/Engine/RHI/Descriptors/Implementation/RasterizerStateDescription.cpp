#include <RHI/Descriptors/RasterizerStateDescription.h>

const RHIRasterizerStateDescription RHIRasterizerStateDescription::Default = RHIRasterizerStateDescription{
  RHIFaceCullMode::Back,     // CullMode
  RHIPolygonFillMode::Solid, // FillMode
  RHIFrontFace::Clockwise,   // FrontFace
  true,                      // DepthClipEnabled
  false                      // ScissorTestEnabled
};

const RHIRasterizerStateDescription RHIRasterizerStateDescription::CullNone = RHIRasterizerStateDescription{
  RHIFaceCullMode::None,     // CullMode
  RHIPolygonFillMode::Solid, // FillMode
  RHIFrontFace::Clockwise,   // FrontFace
  true,                      // DepthClipEnabled
  false                      // ScissorTestEnabled
};


EZ_STATICLINK_FILE(RHI, RHI_Descriptors_Implementation_RasterizerStateDescription);

