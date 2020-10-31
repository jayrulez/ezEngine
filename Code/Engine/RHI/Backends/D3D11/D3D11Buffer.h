#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <Foundation/Algorithm/HashableStruct.h>
#include <RHI/Resources/Buffer.h>

struct ID3D11Device;
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;
struct ID3D11UnorderedAccessView;

struct OffsetSizePair : public ezHashableStruct<OffsetSizePair>
{
  ezUInt32 Offset = 0;

  ezUInt32 Size = 0;

  OffsetSizePair() = default;

  OffsetSizePair(ezUInt32 offset, ezUInt32 size)
  {
    Offset = offset;
    Size = size;
  }

  bool operator==(const OffsetSizePair& other) const
  {
    return Offset == other.Offset && Size == other.Size;
  }
};

class D3D11DeviceBuffer : public RHIBuffer
{
public:
private:
  ID3D11Device* Device = nullptr;
  ID3D11Buffer* Buffer = nullptr;
  ezMutex AccessViewMutex;

  ezUInt32 Size = 0;
  ezBitflags<RHIBufferUsage> Usage;

  ezHashTable<OffsetSizePair, ID3D11ShaderResourceView*> SRVs;
  ezHashTable<OffsetSizePair, ID3D11UnorderedAccessView*> UAVs;
  ezUInt32 StructureByteStride = 0;
  bool RawBuffer = false;

  bool Disposed = false;
  ezString Name;

public:
  virtual ezString GetName() const override;

  virtual void SetName(const ezString& name) override;

  virtual bool IsDisposed() const override;
  virtual void Dispose() override;

  virtual ezUInt32 GetSize() const override;

  virtual ezBitflags<RHIBufferUsage> GetUsage() const override;

  ID3D11Buffer* GetBuffer() const;

public:
  D3D11DeviceBuffer(ID3D11Device* device, ezUInt32 size, ezBitflags<RHIBufferUsage> usage, ezUInt32 structureByteStride, bool rawBuffer);

  //private:
  ID3D11ShaderResourceView* GetShaderResourceView(ezUInt32 offset, ezUInt32 size);

  ID3D11UnorderedAccessView* GetUnorderedAccessView(ezUInt32 offset, ezUInt32 size);


  ID3D11ShaderResourceView* CreateShaderResourceView(ezUInt32 offset, ezUInt32 size);


  ID3D11UnorderedAccessView* CreateUnorderedAccessView(ezUInt32 offset, ezUInt32 size);
};
