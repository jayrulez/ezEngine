#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Resources/Buffer.h>
#include <RHI/Resources/Resource.h>

/// <summary>
/// A <see cref="RHIResourceFlags.Bindable"/> that represents a section of a <see cref="RHIBuffer"/>. This can be used in place of
/// a <see cref="RHIBuffer"/> when creating a <see cref="RHIResourceSet"/> to make only a subset of the Buffer available to
/// shaders.
/// </summary>
struct RHIBufferRange : public RHIResource
{
  /// <summary>
  /// The underlying <see cref="RHIBuffer"/> that this range refers to.
  /// </summary>
  RHIBuffer* Buffer;

  /// <summary>
  /// The offset, in bytes, from the beginning of the buffer that this range starts at.
  /// </summary>
  ezUInt32 Offset;

  /// <summary>
  /// The total number of bytes that this range encompasses.
  /// </summary>
  ezUInt32 Size;

  /// <summary>
  /// Constructs a new <see cref="RHIBufferRange"/>.
  /// </summary>
  /// <param name="buffer">The underlying <see cref="RHIBuffer"/> that this range will refer to.</param>
  /// <param name="offset">The offset, in bytes, from the beginning of the buffer that this range will start at.</param>
  /// <param name="sizeInBytes">The total number of bytes that this range will encompass.</param>
  RHIBufferRange(RHIBuffer* buffer, ezUInt32 offset, ezUInt32 sizeInBytes);

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements are equal; false otherswise.</returns>
  bool operator==(const RHIBufferRange& other) const
  {
    return Buffer == other.Buffer && Offset == other.Offset && Size == other.Size;
  }

  virtual void SetName(const ezString& name) override;

  virtual ezString GetName() const override;

  virtual bool IsDisposed() const override;

  virtual void Dispose() override;

private:
  bool Disposed = false;
};
