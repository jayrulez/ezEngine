#include <RHI/Descriptors/BlendAttachmentDescription.h>

const RHIBlendAttachmentDescription RHIBlendAttachmentDescription::OverrideBlend = RHIBlendAttachmentDescription{
  true,                  // BlendEnabled
  RHIBlendFactor::One,   // SourceColorFactor
  RHIBlendFactor::Zero,  // DestinationColorFactor
  RHIBlendFunction::Add, // ColorFunction
  RHIBlendFactor::One,   // SourceAlphaFactor
  RHIBlendFactor::Zero,  // DestinationAlphaFactor
  RHIBlendFunction::Add  // AlphaFunction
};

const RHIBlendAttachmentDescription RHIBlendAttachmentDescription::AlphaBlend = RHIBlendAttachmentDescription{
  true,                               // BlendEnabled
  RHIBlendFactor::SourceAlpha,        // SourceColorFactor
  RHIBlendFactor::InverseSourceAlpha, // DestinationColorFactor
  RHIBlendFunction::Add,              // ColorFunction
  RHIBlendFactor::SourceAlpha,        // SourceAlphaFactor
  RHIBlendFactor::InverseSourceAlpha, // DestinationAlphaFactor
  RHIBlendFunction::Add               // AlphaFunction
};

const RHIBlendAttachmentDescription RHIBlendAttachmentDescription::AdditiveBlend = RHIBlendAttachmentDescription{
  true,                        // BlendEnabled
  RHIBlendFactor::SourceAlpha, // SourceColorFactor
  RHIBlendFactor::One,         // DestinationColorFactor
  RHIBlendFunction::Add,       // ColorFunction
  RHIBlendFactor::SourceAlpha, // SourceAlphaFactor
  RHIBlendFactor::One,         // DestinationAlphaFactor
  RHIBlendFunction::Add        // AlphaFunction
};

const RHIBlendAttachmentDescription RHIBlendAttachmentDescription::Disabled = RHIBlendAttachmentDescription{
  false,                 // BlendEnabled
  RHIBlendFactor::One,   // SourceColorFactor
  RHIBlendFactor::Zero,  // DestinationColorFactor
  RHIBlendFunction::Add, // ColorFunction
  RHIBlendFactor::One,   // SourceAlphaFactor
  RHIBlendFactor::Zero,  // DestinationAlphaFactor
  RHIBlendFunction::Add  // AlphaFunction
};
