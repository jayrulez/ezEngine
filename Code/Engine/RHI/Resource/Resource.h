#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Basics.h>

struct EZ_RHI_DLL ezRHIMemoryRequirements
{
  ezUInt64 m_Size;
  ezUInt64 m_Alignment;
  ezUInt32 m_MemoryTypeBits;
};

class EZ_RHI_DLL ezRHIResource : public ezRHIQueryInterface
{
public:
  virtual ~ezRHIResource() = default;
  virtual void CommitMemory(ezRHIMemoryType memoryType) = 0;
  virtual void BindMemory(ezRHIMemory* pMemory, ezUInt64 offset) = 0;
  virtual ezRHIResourceType GetResourceType() const = 0;
  virtual ezRHIResourceFormat::Enum GetFormat() const = 0;
  virtual ezRHIMemoryType GetMemoryType() const = 0;
  virtual ezUInt64 GetWidth() const = 0;
  virtual ezUInt32 GetHeight() const = 0;
  virtual ezUInt16 GetLayerCount() const = 0;
  virtual ezUInt16 GetLevelCount() const = 0;
  virtual ezUInt32 GetSampleCount() const = 0;
  virtual ezUInt64 GetAccelerationStructureHandle() const = 0;
  virtual void SetName(const ezString& name) = 0;
  virtual ezUInt8* Map() = 0;
  virtual void Unmap() = 0;
  virtual void UpdateUploadBuffer(ezUInt64 bufferOffset, const void* pData, ezUInt64 numBytes) = 0;
  virtual void UpdateUploadBufferWithTextureData(ezUInt64 bufferOffset, ezUInt32 bufferRowPitch, ezUInt32 bufferDepthPitch,
    const void* pSrcData, ezUInt32 srcRowPitch, ezUInt32 srcDepthPitch, ezUInt32 numRows, ezUInt32 numSlices) = 0;
  virtual bool AllowCommonStatePromotion(ezBitflags<ezRHIResourceState> stateAfter) = 0;
  virtual ezBitflags<ezRHIResourceState> GetInitialState() const = 0;
  virtual ezRHIMemoryRequirements GetMemoryRequirements() const = 0;
  virtual bool IsBackBuffer() const = 0;
};
