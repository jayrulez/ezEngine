#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <Foundation/Algorithm/HashableStruct.h>

/// <summary>
/// Describes an individual output attachment and its format.
/// </summary>
struct EZ_RHI_DLL RHIOutputAttachmentDescription : public ezHashableStruct<RHIOutputAttachmentDescription>
{
  EZ_DECLARE_POD_TYPE();

  /// <summary>
  /// The format of the <see cref="RHITexture"/> attachment.
  /// </summary>
  ezEnum<RHIPixelFormat> Format;

  RHIOutputAttachmentDescription& operator=(const RHIOutputAttachmentDescription& other)
  {
    Format = other.Format;
    return *this;
  }

  RHIOutputAttachmentDescription() = default;

  /// <summary>
  /// Constructs a new <see cref="RHIOutputAttachmentDescription"/>.
  /// </summary>
  /// <param name="format">THe format of the <see cref="Texture"/> attachment.</param>
  RHIOutputAttachmentDescription(ezEnum<RHIPixelFormat> format)
  {
    Format = format;
  }

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements and all array elements are equal; false otherswise.</returns>
  bool operator==(const RHIOutputAttachmentDescription& other) const
  {
    return Format == other.Format;
  }
};
