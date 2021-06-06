#include <RHI/Resources/ResourceFormats.h>

ezUInt32 ezRHIResourceFormat::GetFormatStride(ezRHIResourceFormat::Enum value)
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

bool ezRHIResourceFormat::IsFormatUnorm(ezRHIResourceFormat::Enum value)
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

bool ezRHIResourceFormat::IsFormatBlockCompressed(ezRHIResourceFormat::Enum value)
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

bool ezRHIResourceFormat::IsFormatStencilSupport(ezRHIResourceFormat::Enum value)
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
