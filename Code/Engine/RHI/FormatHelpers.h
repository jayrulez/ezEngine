#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>


namespace FormatHelpers
{
  ezUInt32 GetSize(ezEnum<RHIPixelFormat> format);

  ezUInt32 GetSize(ezEnum<RHIVertexElementFormat> format);

  ezInt32 GetElementCount(ezEnum<RHIVertexElementFormat> format);

  ezUInt32 GetSampleCount(ezEnum<RHITextureSampleCount> sampleCount);

  bool IsStencilFormat(ezEnum<RHIPixelFormat> format);

  bool IsDepthStencilFormat(ezEnum<RHIPixelFormat> format);

  bool IsCompressedFormat(ezEnum<RHIPixelFormat> format);

  ezUInt32 GetRowPitch(ezUInt32 width, ezEnum<RHIPixelFormat> format);

  ezUInt32 GetBlockSize(ezEnum<RHIPixelFormat> format);

  bool IsFormatViewCompatible(ezEnum<RHIPixelFormat> viewFormat, ezEnum<RHIPixelFormat> realFormat);

  bool IsSrgbCounterpart(ezEnum<RHIPixelFormat> viewFormat, ezEnum<RHIPixelFormat> realFormat);

  ezUInt32 GetNumRows(ezUInt32 height, ezEnum<RHIPixelFormat> format);

  ezUInt32 GetDepthPitch(ezUInt32 rowPitch, ezUInt32 height, ezEnum<RHIPixelFormat> format);

  ezUInt32 GetRegionSize(ezUInt32 width, ezUInt32 height, ezUInt32 depth, ezEnum<RHIPixelFormat> format);

  ezEnum<RHITextureSampleCount> GetSampleCount(ezUInt32 samples);

  ezEnum<RHIPixelFormat> GetViewFamilyFormat(ezEnum<RHIPixelFormat> format);
} // namespace FormatHelpers
