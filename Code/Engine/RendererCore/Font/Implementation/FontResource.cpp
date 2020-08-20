#include <RendererCorePCH.h>

#include <Core/Assets/AssetFileHeader.h>
#include <Foundation/Configuration/Startup.h>
#include <RendererCore/Font/FontResource.h>

ezResult ezRawFont::Serialize(ezStreamWriter& stream) const
{
  ezAssetFileHeader asset;
  asset.Write(stream);

  ezUInt8 uiVersion = 1;
  stream << uiVersion;

  stream << m_Name;
  stream << m_FamilyName;
  stream.WriteMap(m_BitmapPerSize);

  return EZ_SUCCESS;
}

ezResult ezRawFont::Deserialize(ezStreamReader& stream)
{
  ezAssetFileHeader asset;
  asset.Read(stream);

  ezUInt8 uiVersion = 0;
  stream >> uiVersion;

  if (uiVersion != 1)
  {
    ezLog::Error("Unrecognied version of ezFont file.");
    return EZ_FAILURE;
  }

  stream >> m_Name;
  stream >> m_FamilyName;
  stream.ReadMap(m_BitmapPerSize);

  return EZ_SUCCESS;
}

// clang-format off
EZ_BEGIN_SUBSYSTEM_DECLARATION(RendererCore, FontResource)

  BEGIN_SUBSYSTEM_DEPENDENCIES
  "Foundation",
  "Core",
  "TextureResource"
  END_SUBSYSTEM_DEPENDENCIES

  ON_CORESYSTEMS_STARTUP
  {
    ezResourceManager::AllowResourceTypeAcquireDuringUpdateContent<ezFontResource, ezTexture2DResource>();
  }

  ON_CORESYSTEMS_SHUTDOWN
  {
  }

  ON_HIGHLEVELSYSTEMS_STARTUP
  {
  }

  ON_HIGHLEVELSYSTEMS_SHUTDOWN
  {
  }

EZ_END_SUBSYSTEM_DECLARATION;
// clang-format on

//////////////////////////////////////////////////////////////////////////

// clang-format off
EZ_BEGIN_DYNAMIC_REFLECTED_TYPE(ezFontResource, 1, ezRTTIDefaultAllocator<ezFontResource>);
EZ_END_DYNAMIC_REFLECTED_TYPE;

EZ_RESOURCE_IMPLEMENT_COMMON_CODE(ezFontResource);
// clang-format on

ezFontResource::ezFontResource()
  : ezResource(DoUpdate::OnMainThread, 1)
{
  ModifyMemoryUsage().m_uiMemoryCPU = sizeof(ezFontResource);
}

ezResourceLoadDesc ezFontResource::UnloadData(Unload WhatToUnload)
{
  ezResourceLoadDesc res;
  res.m_uiQualityLevelsDiscardable = 0;
  res.m_uiQualityLevelsLoadable = 0;
  res.m_State = ezResourceState::Unloaded;

  for (auto& pair : m_FontDataPerSize)
  {
    ezFontBitmap& bitmap = pair.value;

    for (ezTexture2DResourceHandle& textureHandle : bitmap.m_TexturePages)
    {
      textureHandle.Invalidate();
    }
  }
  m_FontDataPerSize.Clear();
  m_GPUMemory = 0;

  return res;
}

ezResourceLoadDesc ezFontResource::UpdateContent(ezStreamReader* Stream)
{
  EZ_LOG_BLOCK("ezFontResource::UpdateContent", GetResourceDescription().GetData());

  ezFontResourceDescriptor desc;
  ezResourceLoadDesc res;
  res.m_uiQualityLevelsDiscardable = 0;
  res.m_uiQualityLevelsLoadable = 0;

  if (Stream == nullptr)
  {
    res.m_State = ezResourceState::LoadedResourceMissing;
    return res;
  }

  // skip the absolute file path data that the standard file reader writes into the stream
  {
    ezString sAbsFilePath;
    (*Stream) >> sAbsFilePath;
  }

  // load and create font
  ezRawFont rawFont;

  if (rawFont.Deserialize(*Stream).Failed())
  {
    res.m_State = ezResourceState::LoadedResourceMissing;
    return res;
  }

  m_Name = rawFont.m_Name;
  m_FamilyName = rawFont.m_FamilyName;
  m_FontDataPerSize.Clear();

  for (auto pair : rawFont.m_BitmapPerSize)
  {
    ezRawFontBitmap& rawFontBitmap = pair.Value();
    m_FontDataPerSize.Insert(rawFontBitmap.m_Size, std::move(ezFontBitmap()));
    ezUInt32 fontIndex = m_FontDataPerSize.Find(rawFontBitmap.m_Size);
    ezFontBitmap& fontBitmap = m_FontDataPerSize.GetValue(fontIndex);

    fontBitmap.m_TexturePages.Reserve(rawFontBitmap.m_Textures.GetCount());

    fontBitmap.m_Size = rawFontBitmap.m_Size;
    fontBitmap.m_BaselineOffset = rawFontBitmap.m_BaselineOffset;
    fontBitmap.m_LineHeight = rawFontBitmap.m_LineHeight;
    fontBitmap.m_MissingGlyph = rawFontBitmap.m_MissingGlyph;
    fontBitmap.m_SpaceWidth = rawFontBitmap.m_SpaceWidth;
    fontBitmap.m_Characters = rawFontBitmap.m_Characters;
    fontBitmap.m_TexturePages.SetCount(rawFontBitmap.m_Textures.GetCount());

    for (ezUInt32 texturePageIndex = 0; texturePageIndex < rawFontBitmap.m_Textures.GetCount(); texturePageIndex++)
    {
      ezStringBuilder textureResourceId;
      textureResourceId.Format("FontTexture_{0}_size{1}_page{2}", m_Name, rawFontBitmap.m_Size, texturePageIndex);

      ezTexture2DResourceHandle textureHandle = ezResourceManager::GetExistingResource<ezTexture2DResource>(textureResourceId);

      if (!textureHandle.IsValid())
      {
        ezImage& image = rawFontBitmap.m_Textures[texturePageIndex];

        ezTexture2DResourceDescriptor textureDescriptor;
        textureDescriptor.m_SamplerDesc.m_AddressU = ezImageAddressMode::Clamp;
        textureDescriptor.m_SamplerDesc.m_AddressV = ezImageAddressMode::Clamp;
        textureDescriptor.m_SamplerDesc.m_AddressW = ezImageAddressMode::Clamp;

        ezUInt32 uiMemory;
        ezHybridArray<ezGALSystemMemoryDescription, 32> initData;

        ezTexture2DResource::FillOutDescriptor(textureDescriptor, &image, true, image.GetNumMipLevels(), uiMemory, initData);

        m_GPUMemory += uiMemory;

        textureHandle = ezResourceManager::CreateResource<ezTexture2DResource>(textureResourceId, std::move(textureDescriptor));
      }

      fontBitmap.m_TexturePages[texturePageIndex] = textureHandle;
    }
  }

  return CreateResource(std::move(desc));
}

void ezFontResource::UpdateMemoryUsage(MemoryUsage& out_NewMemoryUsage)
{
  out_NewMemoryUsage.m_uiMemoryCPU = sizeof(ezFontResource) + (ezUInt32)m_FontDataPerSize.GetHeapMemoryUsage();

  out_NewMemoryUsage.m_uiMemoryGPU = m_GPUMemory;
}

EZ_RESOURCE_IMPLEMENT_CREATEABLE(ezFontResource, ezFontResourceDescriptor)
{
  ezResourceLoadDesc res;
  res.m_uiQualityLevelsDiscardable = 0;
  res.m_uiQualityLevelsLoadable = 0;
  res.m_State = ezResourceState::Loaded;

  return res;
}

const ezFontBitmap& ezFontResource::GetBitmap(ezUInt32 size) const
{
  if (!m_FontDataPerSize.Contains(size))
    EZ_REPORT_FAILURE("Invalid font size requested.");

  ezUInt32 index = m_FontDataPerSize.Find(size);

  const ezFontBitmap& bitmap = m_FontDataPerSize.GetValue(index);

  return bitmap;
}

ezInt32 ezFontResource::GetClosestSize(ezUInt32 size) const
{
  ezUInt32 lowerBoundIndex = m_FontDataPerSize.LowerBound(size);

  if (lowerBoundIndex == ezInvalidIndex)
    return 0;

  ezUInt32 lowerBoundSize = m_FontDataPerSize.GetValue(lowerBoundIndex).m_Size;

  if (lowerBoundSize == size || lowerBoundIndex == 0)
    return lowerBoundSize;

  ezUInt32 previousSize = m_FontDataPerSize.GetValue(lowerBoundIndex - 1).m_Size;

  if (size - previousSize < lowerBoundSize - size)
  {
    return previousSize;
  }

  return lowerBoundSize;
}

bool ezFontResourceLoader::IsResourceOutdated(const ezResource* pResource) const
{
  if (ezResourceLoaderFromFile::IsResourceOutdated(pResource))
    return true;

  return false;
}
