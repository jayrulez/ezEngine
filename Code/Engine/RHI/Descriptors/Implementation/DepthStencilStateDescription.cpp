#include <RHI/Descriptors/DepthStencilStateDescription.h>

const RHIDepthStencilStateDescription RHIDepthStencilStateDescription::DepthOnlyLessEqual = RHIDepthStencilStateDescription{
  true,                        // DepthTestEnabled
  true,                        // DepthWriteEnabled
  RHIComparisonKind::LessEqual // DepthComparison
};

const RHIDepthStencilStateDescription RHIDepthStencilStateDescription::DepthOnlyLessEqualRead = RHIDepthStencilStateDescription{
  true,                        // DepthTestEnabled
  false,                       // DepthWriteEnabled
  RHIComparisonKind::LessEqual // DepthComparison
};

const RHIDepthStencilStateDescription RHIDepthStencilStateDescription::DepthOnlyGreaterEqual = RHIDepthStencilStateDescription{
  true,                           // DepthTestEnabled
  true,                           // DepthWriteEnabled
  RHIComparisonKind::GreaterEqual // DepthComparison
};

const RHIDepthStencilStateDescription RHIDepthStencilStateDescription::DepthOnlyGreaterEqualRead = RHIDepthStencilStateDescription{
  true,                           // DepthTestEnabled
  false,                          // DepthWriteEnabled
  RHIComparisonKind::GreaterEqual // DepthComparison
};

const RHIDepthStencilStateDescription RHIDepthStencilStateDescription::Disabled = RHIDepthStencilStateDescription{
  false,                       // DepthTestEnabled
  false,                       // DepthWriteEnabled
  RHIComparisonKind::LessEqual // DepthComparison
};
