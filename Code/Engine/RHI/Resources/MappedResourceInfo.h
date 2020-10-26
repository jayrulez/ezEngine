#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Resources/MappedResource.h>

struct RHIMappedResourceInfo
{
  ezInt32 RefCount = 0;
  ezEnum<RHIMapMode> Mode = RHIMapMode::Default;
  RHIMappedResource* Resource = nullptr;
};
