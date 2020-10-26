#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>


#include <RHI/Resources/DeviceResource.h>

/// <summary>
/// A device resource encapsulating a single shader module.
/// See <see cref="RHIShaderDescription"/>.
/// </summary>
class EZ_RHI_DLL RHIShader : public RHIDeviceResource
{
public:
  RHIShader(ezEnum<RHIShaderStages> stage, const ezString& entryPoint)
  {
    Stage = stage;
    EntryPoint = entryPoint;
  }

  /// <summary>
  /// The shader stage this instance can be used in.
  /// </summary>
  ezEnum<RHIShaderStages> GetStage() const
  {
    return Stage;
  }

  /// <summary>
  /// The name of the entry point function.
  /// </summary>
  ezString GetEntryPoint() const
  {
    return EntryPoint;
  }

private:
  ezEnum<RHIShaderStages> Stage;
  ezString EntryPoint;
};


