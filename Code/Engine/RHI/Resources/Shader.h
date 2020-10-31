#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>


#include <RHI/Resources/Resource.h>

/// <summary>
/// A device resource encapsulating a single shader module.
/// See <see cref="RHIShaderDescription"/>.
/// </summary>
class EZ_RHI_DLL RHIShader : public RHIResource
{
public:
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

protected:
  RHIShader(ezEnum<RHIShaderStages> stage, const ezString& entryPoint)
  {
    Stage = stage;
    EntryPoint = entryPoint;
  }

private:
  ezEnum<RHIShaderStages> Stage;
  ezString EntryPoint;
};
