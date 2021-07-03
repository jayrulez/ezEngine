
// static
EZ_ALWAYS_INLINE ezUInt32 ezRHIResourceFormat::GetBitsPerElement(ezRHIResourceFormat::Enum format)
{
  return s_BitsPerElement[format];
}

// static
EZ_ALWAYS_INLINE ezUInt8 ezRHIResourceFormat::GetChannelCount(ezRHIResourceFormat::Enum format)
{
  return s_ChannelCount[format];
}

// static
EZ_FORCE_INLINE bool ezRHIResourceFormat::IsDepthFormat(ezRHIResourceFormat::Enum format)
{
  return format == DFloat || format == D16 || format == D24S8;
}

// static
EZ_FORCE_INLINE bool ezRHIResourceFormat::IsSrgb(ezRHIResourceFormat::Enum format)
{
  return format == BGRAUByteNormalizedsRGB || format == RGBAUByteNormalizedsRGB || format == BC1sRGB || format == BC2sRGB || format == BC3sRGB ||
         format == BC7UNormalizedsRGB;
}


template <typename NativeFormatType, NativeFormatType InvalidFormat>
ezRHIFormatLookupEntry<NativeFormatType, InvalidFormat>::ezRHIFormatLookupEntry()
  : m_eStorage(InvalidFormat)
  , m_eRenderTarget(InvalidFormat)
  , m_eDepthOnlyType(InvalidFormat)
  , m_eStencilOnlyType(InvalidFormat)
  , m_eDepthStencilType(InvalidFormat)
  , m_eVertexAttributeType(InvalidFormat)
  , m_eResourceViewType(InvalidFormat)
{
}


template <typename NativeFormatType, NativeFormatType InvalidFormat>
ezRHIFormatLookupEntry<NativeFormatType, InvalidFormat>::ezRHIFormatLookupEntry(NativeFormatType Storage)
  : m_eStorage(Storage)
  , m_eRenderTarget(InvalidFormat)
  , m_eDepthOnlyType(InvalidFormat)
  , m_eStencilOnlyType(InvalidFormat)
  , m_eDepthStencilType(InvalidFormat)
  , m_eVertexAttributeType(InvalidFormat)
  , m_eResourceViewType(InvalidFormat)
{
}

template <typename NativeFormatType, NativeFormatType InvalidFormat>
ezRHIFormatLookupEntry<NativeFormatType, InvalidFormat>& ezRHIFormatLookupEntry<NativeFormatType, InvalidFormat>::RT(
  NativeFormatType RenderTargetType)
{
  m_eRenderTarget = RenderTargetType;
  return *this;
}

template <typename NativeFormatType, NativeFormatType InvalidFormat>
ezRHIFormatLookupEntry<NativeFormatType, InvalidFormat>& ezRHIFormatLookupEntry<NativeFormatType, InvalidFormat>::D(NativeFormatType DepthOnlyType)
{
  m_eDepthOnlyType = DepthOnlyType;
  return *this;
}

template <typename NativeFormatType, NativeFormatType InvalidFormat>
ezRHIFormatLookupEntry<NativeFormatType, InvalidFormat>& ezRHIFormatLookupEntry<NativeFormatType, InvalidFormat>::S(NativeFormatType StencilOnlyType)
{
  m_eStencilOnlyType = StencilOnlyType;
  return *this;
}

template <typename NativeFormatType, NativeFormatType InvalidFormat>
ezRHIFormatLookupEntry<NativeFormatType, InvalidFormat>& ezRHIFormatLookupEntry<NativeFormatType, InvalidFormat>::DS(
  NativeFormatType DepthStencilType)
{
  m_eDepthStencilType = DepthStencilType;
  return *this;
}

template <typename NativeFormatType, NativeFormatType InvalidFormat>
ezRHIFormatLookupEntry<NativeFormatType, InvalidFormat>& ezRHIFormatLookupEntry<NativeFormatType, InvalidFormat>::VA(
  NativeFormatType VertexAttributeType)
{
  m_eVertexAttributeType = VertexAttributeType;
  return *this;
}

template <typename NativeFormatType, NativeFormatType InvalidFormat>
ezRHIFormatLookupEntry<NativeFormatType, InvalidFormat>& ezRHIFormatLookupEntry<NativeFormatType, InvalidFormat>::RV(
  NativeFormatType ResourceViewType)
{
  m_eResourceViewType = ResourceViewType;
  return *this;
}


template <typename FormatClass>
ezRHIFormatLookupTable<FormatClass>::ezRHIFormatLookupTable()
{
  for (ezUInt32 i = 0; i < ezRHIResourceFormat::ENUM_COUNT; i++)
  {
    m_Formats[i] = FormatClass();
  }
}

template <typename FormatClass>
const FormatClass& ezRHIFormatLookupTable<FormatClass>::GetFormatInfo(ezRHIResourceFormat::Enum eFormat) const
{
  return m_Formats[eFormat];
}

template <typename FormatClass>
void ezRHIFormatLookupTable<FormatClass>::SetFormatInfo(ezRHIResourceFormat::Enum eFormat, const FormatClass& NewFormatInfo)
{
  m_Formats[eFormat] = NewFormatInfo;
}
