#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Basics.h>

class ezRHIView : public ezRHIQueryInterface
{
public:
  virtual ~ezRHIView() = default;
  virtual ezRHIResource* GetResource() = 0;
  virtual ezUInt32 GetDescriptorId() const = 0;
  virtual ezUInt32 GetBaseMipLevel() const = 0;
  virtual ezUInt32 GetLevelCount() const = 0;
  virtual ezUInt32 GetBaseArrayLayer() const = 0;
  virtual ezUInt32 GetLayerCount() const = 0;
};
