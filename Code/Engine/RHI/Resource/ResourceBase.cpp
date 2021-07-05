#include <RHIPCH.h>

#include <RHI/Resource/ResourceBase.h>

ezRHIResourceBase::ezRHIResourceBase()
  : m_ResourceStateTracker{*this}
{
}

ezRHIResourceType ezRHIResourceBase::GetResourceType() const
{
  return m_ResourceType;
}

ezRHIResourceFormat::Enum ezRHIResourceBase::GetFormat() const
{
  return m_Format;
}

ezRHIMemoryType ezRHIResourceBase::GetMemoryType() const
{
  return m_MemoryType;
}

void ezRHIResourceBase::UpdateUploadBuffer(ezUInt64 bufferOffset, const void* pData, ezUInt64 numBytes)
{
  void* pDstData = Map() + bufferOffset;
  memcpy(pDstData, pData, numBytes);
  Unmap();
}

void ezRHIResourceBase::UpdateUploadBufferWithTextureData(ezUInt64 bufferOffset, ezUInt32 bufferRowPitch, ezUInt32 bufferDepthPitch, const void* pSrcData, ezUInt32 srcRowPitch, ezUInt32 srcDepthPitch, ezUInt32 numRows, ezUInt32 numSlices)
{
  void* pDstData = Map() + bufferOffset;
  for (ezUInt32 z = 0; z < numSlices; ++z)
  {
    ezUInt8* pDestSlice = reinterpret_cast<ezUInt8*>(pDstData) + bufferDepthPitch * z;
    const ezUInt8* pSrcSlice = reinterpret_cast<const ezUInt8*>(pSrcData) + srcDepthPitch * z;
    for (ezUInt32 y = 0; y < numRows; ++y)
    {
      memcpy(pDestSlice + bufferRowPitch * y, pSrcSlice + srcRowPitch * y, srcRowPitch);
    }
  }
  Unmap();
}

ezBitflags<ezRHIResourceState> ezRHIResourceBase::GetInitialState() const
{
  return m_InitialState;
}

bool ezRHIResourceBase::IsBackBuffer() const
{
  return m_IsBackBuffer;
}

void ezRHIResourceBase::SetInitialState(ezBitflags<ezRHIResourceState> state)
{
  m_InitialState = state;
  m_ResourceStateTracker.SetResourceState(m_InitialState);
}

ezRHIResourceStateTracker& ezRHIResourceBase::GetGlobalResourceStateTracker()
{
  return m_ResourceStateTracker;
}

const ezRHIResourceStateTracker& ezRHIResourceBase::GetGlobalResourceStateTracker() const
{
  return m_ResourceStateTracker;
}
