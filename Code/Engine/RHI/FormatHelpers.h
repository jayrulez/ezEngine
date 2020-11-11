#pragma once
#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

namespace FormatHelpers
{
  ezUInt32 GetFormatStride(ezEnum<ezRHIFormat> value);

  bool IsFormatUnorm(ezEnum<ezRHIFormat> value);

  bool IsFormatBlockCompressed(ezEnum<ezRHIFormat> value);

  bool IsFormatStencilSupport(ezEnum<ezRHIFormat> value);
} // namespace Formathelpers