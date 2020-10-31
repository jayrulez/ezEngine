#include <RHI/Resources/BufferRange.h>

/// <summary>
/// Constructs a new <see cref="RHIBufferRange"/>.
/// </summary>
/// <param name="buffer">The underlying <see cref="RHIBuffer"/> that this range will refer to.</param>
/// <param name="offset">The offset, in bytes, from the beginning of the buffer that this range will start at.</param>
/// <param name="sizeInBytes">The total number of bytes that this range will encompass.</param>

RHIBufferRange::RHIBufferRange(RHIBuffer* buffer, ezUInt32 offset, ezUInt32 sizeInBytes)
{
  Buffer = buffer;
  Offset = offset;
  Size = sizeInBytes;
}

void RHIBufferRange::SetName(const ezString& name)
{
}

ezString RHIBufferRange::GetName() const
{
  return ezString();
}

bool RHIBufferRange::IsDisposed() const
{
  return Disposed;
}

void RHIBufferRange::Dispose()
{
  if (!Disposed)
  {
    Disposed = true;
  }
}
