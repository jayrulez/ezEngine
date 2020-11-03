#include <RHI/Descriptors/SamplerDescription.h>

const RHISamplerDescription RHISamplerDescription::Point = RHISamplerDescription{
  RHISamplerAddressMode::Wrap,                  // AddressModeU
  RHISamplerAddressMode::Wrap,                  // AddressModeV
  RHISamplerAddressMode::Wrap,                  // AddressModeW
  RHISamplerFilter::MinPoint_MagPoint_MipPoint, // Filter
  std::nullopt,                                 // ComparisonKind
  0,                                            // MaximumAnisotropy
  0,                                            // MinimumLod
  ezMath::MaxValue<ezUInt32>(),                 // MaximumLod
  0,                                            // LodBias,
  RHISamplerBorderColor::Default                // BorderColor
};

const RHISamplerDescription RHISamplerDescription::Linear = RHISamplerDescription{
  RHISamplerAddressMode::Wrap,                     // AddressModeU
  RHISamplerAddressMode::Wrap,                     // AddressModeV
  RHISamplerAddressMode::Wrap,                     // AddressModeW
  RHISamplerFilter::MinLinear_MagLinear_MipLinear, // Filter
  std::nullopt,                                    // ComparisonKind
  0,                                               // MaximumAnisotropy
  0,                                               // MinimumLod
  ezMath::MaxValue<ezUInt32>(),                    // MaximumLod
  0,                                               // LodBias,
  RHISamplerBorderColor::Default                   // BorderColor
};

const RHISamplerDescription RHISamplerDescription::Aniso4x = RHISamplerDescription{
  RHISamplerAddressMode::Wrap,   // AddressModeU
  RHISamplerAddressMode::Wrap,   // AddressModeV
  RHISamplerAddressMode::Wrap,   // AddressModeW
  RHISamplerFilter::Anisotropic, // Filter
  std::nullopt,                  // ComparisonKind
  4,                             // MaximumAnisotropy
  0,                             // MinimumLod
  ezMath::MaxValue<ezUInt32>(),  // MaximumLod
  0,                             // LodBias,
  RHISamplerBorderColor::Default // BorderColor
};


EZ_STATICLINK_FILE(RHI, RHI_Descriptors_Implementation_SamplerDescription);

