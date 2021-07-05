EZ_ALWAYS_INLINE ezUInt32 ezRHIResourceFormat::GetFormatStride(ezRHIResourceFormat::Enum value)
{
  switch (value)
  {

    case R32G32B32A32_FLOAT:
    case R32G32B32A32_UINT:
    case R32G32B32A32_SINT:
    case BC1_UNORM:
    case BC1_UNORM_SRGB:
    case BC2_UNORM:
    case BC2_UNORM_SRGB:
    case BC3_UNORM:
    case BC3_UNORM_SRGB:
    case BC4_SNORM:
    case BC4_UNORM:
    case BC5_SNORM:
    case BC5_UNORM:
    case BC6H_UF16:
    case BC6H_SF16:
    case BC7_UNORM:
    case BC7_UNORM_SRGB:
      return 16;

    case R32G32B32_FLOAT:
    case R32G32B32_UINT:
    case R32G32B32_SINT:
      return 12;

    case R16G16B16A16_FLOAT:
    case R16G16B16A16_UNORM:
    case R16G16B16A16_UINT:
    case R16G16B16A16_SNORM:
    case R16G16B16A16_SINT:
      return 8;

    case R32G32_FLOAT:
    case R32G32_UINT:
    case R32G32_SINT:
    case R32G8X24_TYPELESS:
    case D32_FLOAT_S8X24_UINT:
      return 8;

    case R10G10B10A2_UNORM:
    case R10G10B10A2_UINT:
    case R11G11B10_FLOAT:
    case R8G8B8A8_UNORM:
    case R8G8B8A8_UNORM_SRGB:
    case R8G8B8A8_UINT:
    case R8G8B8A8_SNORM:
    case R8G8B8A8_SINT:
    case B8G8R8A8_UNORM:
    case B8G8R8A8_UNORM_SRGB:
    case R16G16_FLOAT:
    case R16G16_UNORM:
    case R16G16_UINT:
    case R16G16_SNORM:
    case R16G16_SINT:
    case R32_TYPELESS:
    case D32_FLOAT:
    case R32_FLOAT:
    case R32_UINT:
    case R32_SINT:
    case R24G8_TYPELESS:
    case D24_UNORM_S8_UINT:
      return 4;

    case R8G8_UNORM:
    case R8G8_UINT:
    case R8G8_SNORM:
    case R8G8_SINT:
    case R16_TYPELESS:
    case R16_FLOAT:
    case D16_UNORM:
    case R16_UNORM:
    case R16_UINT:
    case R16_SNORM:
    case R16_SINT:
      return 2;

    case R8_UNORM:
    case R8_UINT:
    case R8_SNORM:
    case R8_SINT:
      return 1;


    default:
      EZ_ASSERT_ALWAYS(false, "Unknown format"); // didn't catch format!
      break;
  }

  return 16;
}

EZ_ALWAYS_INLINE ezUInt32 ezRHIResourceFormat::GetBlockSize(ezRHIResourceFormat::Enum value)
{
  switch (value)
  {

    case BC1_UNORM:
    case BC1_UNORM_SRGB:
    case BC4_UNORM:
    case BC4_SNORM:
      return 8;

    case BC2_UNORM:
    case BC2_UNORM_SRGB:
    case BC3_UNORM:
    case BC3_UNORM_SRGB:
    case BC5_UNORM:
    case BC5_SNORM:
    case BC6H_UF16:
    case BC6H_SF16:
    case BC7_UNORM:
    case BC7_UNORM_SRGB:
      return 16;


    default:
      EZ_ASSERT_ALWAYS(false, "Invalid block format"); // didn't catch format!
      break;
  }

  return 0;
}

EZ_ALWAYS_INLINE ezVec3U32 ezRHIResourceFormat::GetBlockExtent(ezRHIResourceFormat::Enum value)
{
  static ezMap<ezRHIResourceFormat::Enum, FormatInfo> s_FormatInfos;
  if (s_FormatInfos.GetCount() == 0)
  {
    s_FormatInfos.Insert(ezRHIResourceFormat::R32G32B32A32_FLOAT, FormatInfo{16, ezVec3U32(1, 1, 1), 3});
    s_FormatInfos.Insert(ezRHIResourceFormat::R32G32B32A32_UINT, FormatInfo{16, ezVec3U32(1, 1, 1), 3});
    s_FormatInfos.Insert(ezRHIResourceFormat::R32G32B32A32_SINT, FormatInfo{16, ezVec3U32(1, 1, 1), 3});

    s_FormatInfos.Insert(ezRHIResourceFormat::R32G32B32_FLOAT, FormatInfo{12, ezVec3U32(1, 1, 1), 3});
    s_FormatInfos.Insert(ezRHIResourceFormat::R32G32B32_UINT, FormatInfo{12, ezVec3U32(1, 1, 1), 3});
    s_FormatInfos.Insert(ezRHIResourceFormat::R32G32B32_SINT, FormatInfo{12, ezVec3U32(1, 1, 1), 3});

    s_FormatInfos.Insert(ezRHIResourceFormat::R16G16B16A16_FLOAT, FormatInfo{8, ezVec3U32(1, 1, 1), 4});
    s_FormatInfos.Insert(ezRHIResourceFormat::R16G16B16A16_UNORM, FormatInfo{8, ezVec3U32(1, 1, 1), 4});
    s_FormatInfos.Insert(ezRHIResourceFormat::R16G16B16A16_UINT, FormatInfo{8, ezVec3U32(1, 1, 1), 4});
    s_FormatInfos.Insert(ezRHIResourceFormat::R16G16B16A16_SNORM, FormatInfo{8, ezVec3U32(1, 1, 1), 4});
    s_FormatInfos.Insert(ezRHIResourceFormat::R16G16B16A16_SINT, FormatInfo{8, ezVec3U32(1, 1, 1), 4});

    s_FormatInfos.Insert(ezRHIResourceFormat::R32G32_FLOAT, FormatInfo{8, ezVec3U32(1, 1, 1), 2});
    s_FormatInfos.Insert(ezRHIResourceFormat::R32G32_UINT, FormatInfo{8, ezVec3U32(1, 1, 1), 2});
    s_FormatInfos.Insert(ezRHIResourceFormat::R32G32_SINT, FormatInfo{8, ezVec3U32(1, 1, 1), 2});

    //s_FormatInfos.Insert(ezRHIResourceFormat::R32G8X24_TYPELESS, FormatInfo{0, ezVec3U32(), 0});
    s_FormatInfos.Insert(ezRHIResourceFormat::D32_FLOAT_S8X24_UINT, FormatInfo{5, ezVec3U32(1, 1, 1), 2});

    s_FormatInfos.Insert(ezRHIResourceFormat::R10G10B10A2_UNORM, FormatInfo{4, ezVec3U32(1, 1, 1), 4});
    s_FormatInfos.Insert(ezRHIResourceFormat::R10G10B10A2_UINT, FormatInfo{4, ezVec3U32(1, 1, 1), 4});

    s_FormatInfos.Insert(ezRHIResourceFormat::R11G11B10_FLOAT, FormatInfo{4, ezVec3U32(1, 1, 1), 3});
    s_FormatInfos.Insert(ezRHIResourceFormat::R8G8B8A8_UNORM, FormatInfo{4, ezVec3U32(1, 1, 1), 4});
    s_FormatInfos.Insert(ezRHIResourceFormat::R8G8B8A8_UNORM_SRGB, FormatInfo{4, ezVec3U32(1, 1, 1), 4});
    s_FormatInfos.Insert(ezRHIResourceFormat::R8G8B8A8_UINT, FormatInfo{4, ezVec3U32(1, 1, 1), 4});
    s_FormatInfos.Insert(ezRHIResourceFormat::R8G8B8A8_SNORM, FormatInfo{4, ezVec3U32(1, 1, 1), 4});
    s_FormatInfos.Insert(ezRHIResourceFormat::R8G8B8A8_SINT, FormatInfo{4, ezVec3U32(1, 1, 1), 4});
    s_FormatInfos.Insert(ezRHIResourceFormat::B8G8R8A8_UNORM, FormatInfo{4, ezVec3U32(1, 1, 1), 4});
    s_FormatInfos.Insert(ezRHIResourceFormat::B8G8R8A8_UNORM_SRGB, FormatInfo{4, ezVec3U32(1, 1, 1), 4});
    s_FormatInfos.Insert(ezRHIResourceFormat::R16G16_FLOAT, FormatInfo{4, ezVec3U32(1, 1, 1), 2});
    s_FormatInfos.Insert(ezRHIResourceFormat::R16G16_UNORM, FormatInfo{4, ezVec3U32(1, 1, 1), 2});
    s_FormatInfos.Insert(ezRHIResourceFormat::R16G16_UINT, FormatInfo{4, ezVec3U32(1, 1, 1), 2});
    s_FormatInfos.Insert(ezRHIResourceFormat::R16G16_SNORM, FormatInfo{4, ezVec3U32(1, 1, 1), 2});
    s_FormatInfos.Insert(ezRHIResourceFormat::R16G16_SINT, FormatInfo{4, ezVec3U32(1, 1, 1), 4});
    //s_FormatInfos.Insert(ezRHIResourceFormat::R32_TYPELESS, FormatInfo{0, ezVec3U32(), 0});
    s_FormatInfos.Insert(ezRHIResourceFormat::D32_FLOAT, FormatInfo{4, ezVec3U32(1, 1, 1), 1});
    s_FormatInfos.Insert(ezRHIResourceFormat::R32_FLOAT, FormatInfo{4, ezVec3U32(1, 1, 1), 1});
    s_FormatInfos.Insert(ezRHIResourceFormat::R32_UINT, FormatInfo{4, ezVec3U32(1, 1, 1), 1});
    s_FormatInfos.Insert(ezRHIResourceFormat::R32_SINT, FormatInfo{4, ezVec3U32(1, 1, 1), 1});
    //s_FormatInfos.Insert(ezRHIResourceFormat::R24G8_TYPELESS, FormatInfo{0, ezVec3U32(), 0});
    s_FormatInfos.Insert(ezRHIResourceFormat::D24_UNORM_S8_UINT, FormatInfo{4, ezVec3U32(1, 1, 1), 2});

    s_FormatInfos.Insert(ezRHIResourceFormat::R8G8_UNORM, FormatInfo{2, ezVec3U32(1, 1, 1), 2});
    s_FormatInfos.Insert(ezRHIResourceFormat::R8G8_UINT, FormatInfo{2, ezVec3U32(1, 1, 1), 2});
    s_FormatInfos.Insert(ezRHIResourceFormat::R8G8_SNORM, FormatInfo{2, ezVec3U32(1, 1, 1), 2});
    s_FormatInfos.Insert(ezRHIResourceFormat::R8G8_SINT, FormatInfo{2, ezVec3U32(1, 1, 1), 2});
    //s_FormatInfos.Insert(ezRHIResourceFormat::R16_TYPELESS, FormatInfo{0, ezVec3U32(), 0});
    s_FormatInfos.Insert(ezRHIResourceFormat::R16_FLOAT, FormatInfo{2, ezVec3U32(1, 1, 1), 1});
    s_FormatInfos.Insert(ezRHIResourceFormat::D16_UNORM, FormatInfo{2, ezVec3U32(1, 1, 1), 1});
    s_FormatInfos.Insert(ezRHIResourceFormat::R16_UNORM, FormatInfo{2, ezVec3U32(1, 1, 1), 1});
    s_FormatInfos.Insert(ezRHIResourceFormat::R16_UINT, FormatInfo{2, ezVec3U32(1, 1, 1), 1});
    s_FormatInfos.Insert(ezRHIResourceFormat::R16_SNORM, FormatInfo{2, ezVec3U32(1, 1, 1), 1});
    s_FormatInfos.Insert(ezRHIResourceFormat::R16_SINT, FormatInfo{2, ezVec3U32(1, 1, 1), 1});

    s_FormatInfos.Insert(ezRHIResourceFormat::R8_UNORM, FormatInfo{1, ezVec3U32(1,1,1), 1});
    s_FormatInfos.Insert(ezRHIResourceFormat::R8_UINT, FormatInfo{1, ezVec3U32(1, 1, 1), 1});
    s_FormatInfos.Insert(ezRHIResourceFormat::R8_SNORM, FormatInfo{1, ezVec3U32(1, 1, 1), 1});
    s_FormatInfos.Insert(ezRHIResourceFormat::R8_SINT, FormatInfo{1, ezVec3U32(1, 1, 1), 1});

    s_FormatInfos.Insert(ezRHIResourceFormat::BC1_UNORM, FormatInfo{8, ezVec3U32(4,4,1), 4});
    s_FormatInfos.Insert(ezRHIResourceFormat::BC1_UNORM_SRGB, FormatInfo{8, ezVec3U32(4,4,1), 4});
    s_FormatInfos.Insert(ezRHIResourceFormat::BC2_UNORM, FormatInfo{16, ezVec3U32(4,4,1), 4});
    s_FormatInfos.Insert(ezRHIResourceFormat::BC2_UNORM_SRGB, FormatInfo{16, ezVec3U32(4, 4, 1), 4});
    s_FormatInfos.Insert(ezRHIResourceFormat::BC3_UNORM, FormatInfo{16, ezVec3U32(4, 4, 1), 4});
    s_FormatInfos.Insert(ezRHIResourceFormat::BC3_UNORM_SRGB, FormatInfo{16, ezVec3U32(4, 4, 1), 4});
    s_FormatInfos.Insert(ezRHIResourceFormat::BC4_UNORM, FormatInfo{8, ezVec3U32(4,4,1), 1});
    s_FormatInfos.Insert(ezRHIResourceFormat::BC4_SNORM, FormatInfo{8, ezVec3U32(4, 4, 1), 1});
    s_FormatInfos.Insert(ezRHIResourceFormat::BC5_UNORM, FormatInfo{16, ezVec3U32(4,4,1), 2});
    s_FormatInfos.Insert(ezRHIResourceFormat::BC5_SNORM, FormatInfo{16, ezVec3U32(4, 4, 1), 2});
    s_FormatInfos.Insert(ezRHIResourceFormat::BC6H_UF16, FormatInfo{16, ezVec3U32(4,4,1), 3});
    s_FormatInfos.Insert(ezRHIResourceFormat::BC6H_SF16, FormatInfo{16, ezVec3U32(4,4,1), 3});
    s_FormatInfos.Insert(ezRHIResourceFormat::BC7_UNORM, FormatInfo{16, ezVec3U32(4,4,1), 3});
    s_FormatInfos.Insert(ezRHIResourceFormat::BC7_UNORM_SRGB, FormatInfo{16, ezVec3U32(4,4,1), 3});
  }

  if (s_FormatInfos.Contains(value))
  {
    return s_FormatInfos[value].BlockExtent;
  }

  return {};
}

EZ_ALWAYS_INLINE bool ezRHIResourceFormat::IsFormatUnorm(ezRHIResourceFormat::Enum value)
{
  switch (value)
  {
    case R16G16B16A16_UNORM:
    case R10G10B10A2_UNORM:
    case R8G8B8A8_UNORM:
    case R8G8B8A8_UNORM_SRGB:
    case B8G8R8A8_UNORM:
    case B8G8R8A8_UNORM_SRGB:
    case R16G16_UNORM:
    case D24_UNORM_S8_UINT:
    case R8G8_UNORM:
    case D16_UNORM:
    case R16_UNORM:
    case R8_UNORM:
      return true;
  }

  return false;
}

EZ_ALWAYS_INLINE bool ezRHIResourceFormat::IsFormatBlockCompressed(ezRHIResourceFormat::Enum value)
{
  switch (value)
  {
    case BC1_UNORM:
    case BC1_UNORM_SRGB:
    case BC2_UNORM:
    case BC2_UNORM_SRGB:
    case BC3_UNORM:
    case BC3_UNORM_SRGB:
    case BC4_UNORM:
    case BC4_SNORM:
    case BC5_UNORM:
    case BC5_SNORM:
    case BC6H_UF16:
    case BC6H_SF16:
    case BC7_UNORM:
    case BC7_UNORM_SRGB:
      return true;
  }

  return false;
}

EZ_ALWAYS_INLINE bool ezRHIResourceFormat::IsFormatStencilSupport(ezRHIResourceFormat::Enum value)
{
  switch (value)
  {
    case R32G8X24_TYPELESS:
    case D32_FLOAT_S8X24_UINT:
    case R24G8_TYPELESS:
    case D24_UNORM_S8_UINT:
      return true;
  }

  return false;
}


EZ_ALWAYS_INLINE void ezRHIResourceFormat::GetInfo(ezUInt32 width,
  ezUInt32 height,
  ezRHIResourceFormat::Enum format,
  ezUInt32& numBytes,
  ezUInt32& rowBytes,
  ezUInt32& numRows,
  ezUInt32 alignment)
{
  if (IsFormatBlockCompressed(format))
  {
    rowBytes = GetBlockSize(format) * ((width + GetBlockExtent(format).x - 1) / GetBlockExtent(format).x);
    rowBytes = Align(rowBytes, alignment);
    numRows = ((height + GetBlockExtent(format).y - 1) / GetBlockExtent(format).y);
  }
  else
  {
    rowBytes = width * GetFormatStride(format);
    rowBytes = Align(rowBytes, alignment);
    numRows = height;
  }
  numBytes = rowBytes * numRows;
}

EZ_ALWAYS_INLINE void ezRHIResourceFormat::GetInfo(ezUInt32 width,
  ezUInt32 height,
  ezRHIResourceFormat::Enum format,
  ezUInt32& numBytes,
  ezUInt32& rowBytes)
{
  ezUInt32 num_rows = 0;
  ezUInt32 alignment = 1;
  return GetInfo(width, height, format, numBytes, rowBytes, num_rows, alignment);
}

EZ_ALWAYS_INLINE ezUInt32 ezRHIResourceFormat::Align(ezUInt32 size, ezUInt32 alignment)
{
  return (size + (alignment - 1)) & ~(alignment - 1);
}
