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

#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    EZ_VERIFY(resource->GetFlags().IsSet(RHIResourceFlags::Bindable), "Resource is not bindable.");
#endif

    RHIBufferRange* range = dynamic_cast<RHIBufferRange*>(resource);
    if (range != nullptr)
    {
      return RHIBufferRange(range->GetBuffer(), range->GetOffset() + additionalOffset, range->GetSize());
    }
    else
    {
      RHIBuffer* buffer = static_cast<RHIBuffer*>(resource);

      return RHIBufferRange(buffer, additionalOffset, buffer->GetSize());
    }
  }

  bool GetDeviceBuffer(RHIResource* resource, RHIBuffer* buffer)
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
}
