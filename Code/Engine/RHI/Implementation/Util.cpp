#include <RHI/Util.h>
#include <RHI/Resources/Resource.h>
#include <RHI/Resources/Buffer.h>
#include <RHI/Resources/BufferRange.h>
#include <RHI/Resources/Texture.h>
#include <RHI/Resources/TextureView.h>

namespace Util
{
  void GetMipDimensions(RHITexture* tex, ezUInt32 mipLevel, ezUInt32& width, ezUInt32& height, ezUInt32& depth)
  {
    width = GetDimension(tex->GetWidth(), mipLevel);
    height = GetDimension(tex->GetHeight(), mipLevel);
    depth = GetDimension(tex->GetDepth(), mipLevel);
  }

  ezUInt32 GetDimension(ezUInt32 largestLevelDimension, ezUInt32 mipLevel)
  {
    ezUInt32 ret = largestLevelDimension;
    for (ezUInt32 i = 0; i < mipLevel; i++)
    {
      ret /= 2;
    }

    return ezMath::Max((ezUInt32)1, ret);
  }

  RHIBufferRange GetBufferRange(RHIResource* resource, ezUInt32 additionalOffset)
  {
    RHIBufferRange* range = dynamic_cast<RHIBufferRange*>(resource);
    if (range != nullptr)
    {
      return RHIBufferRange(range->GetBuffer(), range->GetOffset() + additionalOffset, range->GetSize());
    }
    else
    {
      RHIBuffer* buffer = dynamic_cast<RHIBuffer*>(resource);

      return RHIBufferRange(buffer, additionalOffset, buffer->GetSize());
    }
  }

  bool GetDeviceBuffer(RHIResource* resource, RHIBuffer*& buffer)
  {
    RHIBuffer* db = dynamic_cast<RHIBuffer*>(resource);
    RHIBufferRange* dbr = dynamic_cast<RHIBufferRange*>(resource);
    if (db)
    {
      buffer = db;
      return true;
    }
    else if (dbr)
    {
      buffer = dbr->GetBuffer();
      return true;
    }

    buffer = nullptr;
    return false;
  }

  RHITextureView* GetTextureView(RHIGraphicsDevice* gd, RHIResource* resource)
  {
    RHITextureView* view = dynamic_cast<RHITextureView*>(resource);
    RHITexture* tex = dynamic_cast<RHITexture*>(resource);
    if (view)
    {
      return view;
    }
    else if (tex)
    {
      return tex->GetFullTextureView(gd);
    }
    else
    {
      EZ_REPORT_FAILURE("Unexpected resource type. Expected Texture or TextureView but found {resource.GetType().Name}");
      return nullptr;
    }
  }
  void CopyTextureRegion(void* src, ezUInt32 srcX, ezUInt32 srcY, ezUInt32 srcZ, ezUInt32 srcRowPitch, ezUInt32 srcDepthPitch, void* dst, ezUInt32 dstX, ezUInt32 dstY, ezUInt32 dstZ, ezUInt32 dstRowPitch, ezUInt32 dstDepthPitch, ezUInt32 width, ezUInt32 height, ezUInt32 depth, ezEnum<RHIPixelFormat> format)
  {
    ezUInt32 blockSize = FormatHelpers::IsCompressedFormat(format) ? 4u : 1u;
    ezUInt32 blockSizeInBytes = blockSize > 1 ? FormatHelpers::GetBlockSize(format) : FormatHelpers::GetSize(format);
    ezUInt32 compressedSrcX = srcX / blockSize;
    ezUInt32 compressedSrcY = srcY / blockSize;
    ezUInt32 compressedDstX = dstX / blockSize;
    ezUInt32 compressedDstY = dstY / blockSize;
    ezUInt32 numRows = FormatHelpers::GetNumRows(height, format);
    ezUInt32 rowSize = width / blockSize * blockSizeInBytes;

    if (srcRowPitch == dstRowPitch && srcDepthPitch == dstDepthPitch)
    {
      //ezUInt32 totalCopySize = depth * srcDepthPitch;
      //ezMemoryUtils::Copy(
      //  src,
      //  dst,
      //  //totalCopySize,
      //  totalCopySize);
    }
    else
    {
      for (ezUInt32 zz = 0; zz < depth; zz++)
        for (ezUInt32 yy = 0; yy < numRows; yy++)
        {
          ezUInt8* rowCopyDst = (ezUInt8*)dst + dstDepthPitch * (zz + dstZ) + dstRowPitch * (yy + compressedDstY) + blockSizeInBytes * compressedDstX;

          ezUInt8* rowCopySrc = (ezUInt8*)src + srcDepthPitch * (zz + srcZ) + srcRowPitch * (yy + compressedSrcY) + blockSizeInBytes * compressedSrcX;

          ezMemoryUtils::Copy(rowCopyDst, rowCopySrc, rowSize);
        }
    }
  }
}


EZ_STATICLINK_FILE(RHI, RHI_Implementation_Util);

