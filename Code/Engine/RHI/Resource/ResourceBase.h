#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Basics.h>
#include <RHI/Resource/Resource.h>
#include <RHI/Resource/ResourceStateTracker.h>

class EZ_RHI_DLL ezRHIResourceBase : public ezRHIResource
{
public:
  ezRHIResourceBase();

  ezRHIResourceType GetResourceType() const override final;
  ezRHIResourceFormat::Enum GetFormat() const override final;
  ezRHIMemoryType GetMemoryType() const override final;

  void UpdateUploadBuffer(ezUInt64 bufferOffset, const void* pData, ezUInt64 numBytes) override final;
  void UpdateUploadBufferWithTextureData(ezUInt64 bufferOffset, ezUInt32 bufferRowPitch, ezUInt32 bufferDepthPitch,
    const void* pSrcData, ezUInt32 srcRowPitch, ezUInt32 srcDepthPitch, ezUInt32 numRows, ezUInt32 numSlices) override final;
  ezBitflags<ezRHIResourceState> GetInitialState() const override final;
  bool IsBackBuffer() const override final;

  void SetInitialState(ezBitflags<ezRHIResourceState> state);
  ezRHIResourceStateTracker& GetGlobalResourceStateTracker();
  const ezRHIResourceStateTracker& GetGlobalResourceStateTracker() const;

  ezRHIResourceFormat::Enum m_Format = ezRHIResourceFormat::Unknown;
  ezRHIResourceType m_ResourceType = ezRHIResourceType::Unknown;
  ezRHIResource* m_AccelerationStructuresMemory;
  bool m_IsBackBuffer = false;

protected:
  ezRHIMemory* m_Memory;
  ezRHIMemoryType m_MemoryType = ezRHIMemoryType::Default;

private:
  ezRHIResourceStateTracker m_ResourceStateTracker;
  ezBitflags<ezRHIResourceState> m_InitialState = ezRHIResourceState::Unknown;
};
