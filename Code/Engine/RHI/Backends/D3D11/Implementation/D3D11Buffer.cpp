#include <RHI/Backends/D3D11/D3D11Buffer.h>
#include <RHI/Backends/D3D11/D3D11Formats.h>
#include <Foundation/Basics/Platform/Win/IncludeWindows.h>
#include <d3d11_1.h>

ezUInt32 ezHashHelper<OffsetSizePair>::Hash(const OffsetSizePair& value)
{
  ezUInt32 hash = 0;

  hash = ezHashingUtils::xxHash32(&value.Offset, sizeof(value.Offset), hash);
  hash = ezHashingUtils::xxHash32(&value.Size, sizeof(value.Size), hash);

  return hash;
}

void D3D11DeviceBuffer::SetName(const ezString& name)
{
  Name = name;
  for (auto& kvp : SRVs)
  {
    ezStringBuilder sb(name, "_SRV");

    kvp.Value()->SetPrivateData(WKPDID_D3DDebugObjectName, sb.GetCharacterCount(), sb.GetData());
  }
  for (auto& kvp : UAVs)
  {
    ezStringBuilder sb(name, "_UAV");

    kvp.Value()->SetPrivateData(WKPDID_D3DDebugObjectName, sb.GetCharacterCount(), sb.GetData());
  }
}

void D3D11DeviceBuffer::Dispose()
{
  if (!Disposed)
  {
    if (Buffer != nullptr)
    {
      for (auto& kvp : SRVs)
      {
        kvp.Value()->Release();
      }

      for (auto& kvp : UAVs)
      {
        kvp.Value()->Release();
      }

      Buffer->Release();
      Buffer = nullptr;
    }
    Disposed = true;
  }
}

D3D11DeviceBuffer::D3D11DeviceBuffer(ID3D11Device* device, ezUInt32 size, ezBitflags<RHIBufferUsage> usage, ezUInt32 structureByteStride, bool rawBuffer)
{
  Device = device;
  Size = size;
  Usage = usage;
  StructureByteStride = structureByteStride;
  RawBuffer = rawBuffer;

  D3D11_BUFFER_DESC bd;
  bd.ByteWidth = size;
  bd.BindFlags = D3D11Formats::RHIToD3D11BindFlags(usage);
  bd.Usage = D3D11_USAGE_DEFAULT;
  bd.CPUAccessFlags = 0;
  bd.MiscFlags = 0;

  if ((usage & RHIBufferUsage::StructuredBufferReadOnly) == RHIBufferUsage::StructuredBufferReadOnly || (usage & RHIBufferUsage::StructuredBufferReadWrite) == RHIBufferUsage::StructuredBufferReadWrite)
  {
    if (rawBuffer)
    {
      bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
    }
    else
    {
      bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
      bd.StructureByteStride = structureByteStride;
    }
  }
  if ((usage & RHIBufferUsage::IndirectBuffer) == RHIBufferUsage::IndirectBuffer)
  {
    bd.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
  }

  if ((usage & RHIBufferUsage::Dynamic) == RHIBufferUsage::Dynamic)
  {
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  }
  else if ((usage & RHIBufferUsage::Staging) == RHIBufferUsage::Staging)
  {
    bd.Usage = D3D11_USAGE_STAGING;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
  }

  HRESULT hr = device->CreateBuffer(&bd, nullptr, &Buffer);
}

ID3D11ShaderResourceView* D3D11DeviceBuffer::GetShaderResourceView(ezUInt32 offset, ezUInt32 size)
{
  ezLock lock(AccessViewMutex);

  OffsetSizePair pair(offset, size);

  ID3D11ShaderResourceView* srv = nullptr;

  if (!SRVs.TryGetValue(pair, srv))
  {
    srv = CreateShaderResourceView(offset, size);
    SRVs.Insert(pair, srv);
  }

  return srv;
}

ID3D11UnorderedAccessView* D3D11DeviceBuffer::GetUnorderedAccessView(ezUInt32 offset, ezUInt32 size)
{
  ezLock lock(AccessViewMutex);
  OffsetSizePair pair(offset, size);
  ID3D11UnorderedAccessView* uav = nullptr;
  if (!UAVs.TryGetValue(pair, uav))
  {
    uav = CreateUnorderedAccessView(offset, size);
    UAVs.Insert(pair, uav);
  }

  return uav;
}

ID3D11ShaderResourceView* D3D11DeviceBuffer::CreateShaderResourceView(ezUInt32 offset, ezUInt32 size)
{
  ID3D11ShaderResourceView* srv = nullptr;
  HRESULT hr;
  if (RawBuffer)
  {
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
    srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    srvDesc.BufferEx.NumElements = size / 4;
    srvDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
    srvDesc.BufferEx.FirstElement = offset / 4;
    hr = Device->CreateShaderResourceView(Buffer, &srvDesc, &srv);
  }
  else
  {
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.NumElements = (size / StructureByteStride);
    srvDesc.Buffer.ElementOffset = (offset / StructureByteStride);
    hr = Device->CreateShaderResourceView(Buffer, &srvDesc, &srv);
  }

  return srv;
}

ID3D11UnorderedAccessView* D3D11DeviceBuffer::CreateUnorderedAccessView(ezUInt32 offset, ezUInt32 size)
{
  ID3D11UnorderedAccessView* uav = nullptr;
  HRESULT hr;
  if (RawBuffer)
  {
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

    uavDesc.Buffer.NumElements = size / 4;
    uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
    uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    uavDesc.Buffer.FirstElement = offset / 4;

    hr = Device->CreateUnorderedAccessView(Buffer, &uavDesc, &uav);
  }
  else
  {
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

    uavDesc.Buffer.NumElements = (size / StructureByteStride);
    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    uavDesc.Buffer.FirstElement = (offset / StructureByteStride);

    hr = Device->CreateUnorderedAccessView(Buffer, &uavDesc, &uav);
  }

  return uav;
}


EZ_STATICLINK_FILE(RHI, RHI_Backends_D3D11_Implementation_D3D11Buffer);

