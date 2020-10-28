#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Types/Id.h>
#include <Foundation/Types/RefCounted.h>

// Configure the DLL Import/Export Define
#if EZ_ENABLED(EZ_COMPILE_ENGINE_AS_DLL)
#  ifdef BUILDSYSTEM_BUILDING_RHI_LIB
#    define EZ_RHI_DLL __declspec(dllexport)
#  else
#    define EZ_RHI_DLL __declspec(dllimport)
#  endif
#else
#  define EZ_RHI_DLL
#endif

/*
struct EZ_RHI_DLL RHI // : byte
{
  using StorageType = ezUInt8;
  enum Enum
  {
    ENUM_COUNT
  };

  struct Bits
  {
  };
};
EZ_DECLARE_FLAGS_OPERATORS(RHIType);
*/

/// <summary>
/// The format of index data used in a <see cref="RHIDeviceBuffer"/>.
/// </summary>
struct EZ_RHI_DLL RHIIndexFormat // : byte
{
  using StorageType = ezUInt8;
  enum Enum
  {
    /// <summary>
    /// Each index is a 16-bit unsigned integer (ezUInt16).
    /// </summary>
    UInt16,
    /// <summary>
    /// Each index is a 32-bit unsigned integer (ezUInt32).
    /// </summary>
    UInt32,
    Default = UInt16
  };
};

/// <summary>
/// Controls the influence of components in a blend operation.
/// </summary>
struct EZ_RHI_DLL RHIBlendFactor // : byte
{
  using StorageType = ezUInt8;
  enum Enum
  {
    /// <summary>
    /// Each component is multiplied by 0.
    /// </summary>
    Zero,
    /// <summary>
    /// Each component is multiplied by 1.
    /// </summary>
    One,
    /// <summary>
    /// Each component is multiplied by the source alpha component.
    /// </summary>
    SourceAlpha,
    /// <summary>
    /// Each component is multiplied by (1 - source alpha).
    /// </summary>
    InverseSourceAlpha,
    /// <summary>
    /// Each component is multiplied by the destination alpha component.
    /// </summary>
    DestinationAlpha,
    /// <summary>
    /// Each component is multiplied by (1 - destination alpha).
    /// </summary>
    InverseDestinationAlpha,
    /// <summary>
    /// Each component is multiplied by the matching component of the source color.
    /// </summary>
    SourceColor,
    /// <summary>
    /// Each component is multiplied by (1 - the matching component of the source color).
    /// </summary>
    InverseSourceColor,
    /// <summary>
    /// Each component is multiplied by the matching component of the destination color.
    /// </summary>
    DestinationColor,
    /// <summary>
    /// Each component is multiplied by (1 - the matching component of the destination color).
    /// </summary>
    InverseDestinationColor,
    /// <summary>
    /// Each component is multiplied by the matching component in constant factor specified in <see cref="BlendStateDescription.BlendFactor"/>.
    /// </summary>
    BlendFactor,
    /// <summary>
    /// Each component is multiplied by (1 - the matching component in constant factor specified in <see cref="BlendStateDescription.BlendFactor"/>).
    /// </summary>
    InverseBlendFactor,
    Default = Zero
  };
};

/// <summary>
/// Controls how the source and destination factors are combined in a blend operation.
/// </summary>
struct EZ_RHI_DLL RHIBlendFunction // : byte
{
  using StorageType = ezUInt8;
  enum Enum
  {
    /// <summary>
    /// Source and destination are added.
    /// </summary>
    Add,
    /// <summary>
    /// Destination is subtracted from source.
    /// </summary>
    Subtract,
    /// <summary>
    /// Source is subtracted from destination.
    /// </summary>
    ReverseSubtract,
    /// <summary>
    /// The minimum of source and destination is selected.
    /// </summary>
    Minimum,
    /// <summary>
    /// The maximum of source and destination is selected.
    /// </summary>
    Maximum,
    Default = Add
  };
};

/// <summary>
/// A bitmask describing the permitted uses of a <see cref="RHIDeviceBuffer"/> object.
/// </summary>
struct EZ_RHI_DLL RHIBufferUsage // : byte
{
  using StorageType = ezUInt8;
  enum Enum
  {
    None,
    /// <summary>
    /// Indicates that a <see cref="RHIDeviceBuffer"/> can be used as the source of vertex data for drawing commands.
    /// This flag enables the use of a Buffer in the <see cref="RHICommandList.SetVertexBuffer(ezUInt32, RHIDeviceBuffer)"/> method.
    /// </summary>
    VertexBuffer = 1 << 0,
    /// <summary>
    /// Indicates that a <see cref="RHIDeviceBuffer"/> can be used as the source of index data for drawing commands.
    /// This flag enables the use of a Buffer in the <see cref="RHICommandList.SetIndexBuffer(RHIDeviceBuffer, RHIIndexFormat)" /> method.
    /// </summary>
    IndexBuffer = 1 << 1,
    /// <summary>
    /// Indicates that a <see cref="RHIDeviceBuffer"/> can be used as a uniform Buffer.
    /// This flag enables the use of a Buffer in a <see cref="RHIResourceSet"/> as a uniform Buffer.
    /// </summary>
    UniformBuffer = 1 << 2,
    /// <summary>
    /// Indicates that a <see cref="RHIDeviceBuffer"/> can be used as a read-only structured Buffer.
    /// This flag enables the use of a Buffer in a <see cref="RHIResourceSet"/> as a read-only structured Buffer.
    /// This flag can only be combined with <see cref="Dynamic"/>.
    /// </summary>
    StructuredBufferReadOnly = 1 << 3,
    /// <summary>
    /// Indicates that a <see cref="DeviceBuffer"/> can be used as a read-write structured Buffer.
    /// This flag enables the use of a Buffer in a <see cref="ResourceSet"/> as a read-write structured Buffer.
    /// This flag cannot be combined with any other flag.
    /// </summary>
    StructuredBufferReadWrite = 1 << 4,
    /// <summary>
    /// Indicates that a <see cref="RHIDeviceBuffer"/> can be used as the source of indirect drawing information.
    /// This flag enables the use of a Buffer in the *Indirect methods of <see cref="RHICommandList"/>.
    /// This flag cannot be combined with <see cref="Dynamic"/>.
    /// </summary>
    IndirectBuffer = 1 << 5,
    /// <summary>
    /// Indicates that a <see cref="RHIDeviceBuffer"/> will be updated with new data very frequently. Dynamic Buffers can be
    /// mapped with <see cref="RHIMapMode.Write"/>. This flag cannot be combined with <see cref="StructuredBufferReadWrite"/>
    /// or <see cref="IndirectBuffer"/>.
    /// </summary>
    Dynamic = 1 << 6,
    /// <summary>
    /// Indicates that a <see cref="RHIDeviceBuffer"/> will be used as a staging Buffer. Staging Buffers can be used to transfer data
    /// to-and-from the CPU using <see cref="RHIGraphicsDevice.Map(RHIMappableResource, RHIMapMode)"/>. Staging Buffers can use all
    /// <see cref="RHIMapMode"/> values.
    /// This flag cannot be combined with any other flag.
    /// </summary>
    Staging = 1 << 7,
    Default = None
  };

  struct Bits
  {
  };
};
EZ_DECLARE_FLAGS_OPERATORS(RHIBufferUsage);

/// <summary>
/// Describes how new values are compared with existing values in a depth or stencil comparison.
/// </summary>
struct EZ_RHI_DLL RHIComparisonKind // : byte
{
  using StorageType = ezUInt8;
  enum Enum
  {
    /// <summary>
    /// The comparison never succeeds.
    /// </summary>
    Never,
    /// <summary>
    /// The comparison succeeds when the new value is less than the existing value.
    /// </summary>
    Less,
    /// <summary>
    /// The comparison succeeds when the new value is equal to the existing value.
    /// </summary>
    Equal,
    /// <summary>
    /// The comparison succeeds when the new value is less than or equal to the existing value.
    /// </summary>
    LessEqual,
    /// <summary>
    /// The comparison succeeds when the new value is greater than the existing value.
    /// </summary>
    Greater,
    /// <summary>
    /// The comparison succeeds when the new value is not equal to the existing value.
    /// </summary>
    NotEqual,
    /// <summary>
    /// The comparison succeeds when the new value is greater than or equal to the existing value.
    /// </summary>
    GreaterEqual,
    /// <summary>
    /// The comparison always succeeds.
    /// </summary>
    Always,
    Default = Never
  };
};

/// <summary>
/// Indicates which face will be culled.
/// </summary>
struct EZ_RHI_DLL RHIFaceCullMode // : byte
{
  using StorageType = ezUInt8;
  enum Enum
  {
    /// <summary>
    /// The back face.
    /// </summary>
    Back,
    /// <summary>
    /// The front face.
    /// </summary>
    Front,
    /// <summary>
    /// No face culling.
    /// </summary>
    None
  };
};

/// <summary>
/// The winding order used to determine the front face of a primitive.
/// </summary>
struct EZ_RHI_DLL RHIFrontFace // : byte
{
  using StorageType = ezUInt8;
  enum Enum
  {
    /// <summary>
    /// Clockwise winding order.
    /// </summary>
    Clockwise,
    /// <summary>
    /// Counter-clockwise winding order.
    /// </summary>
    CounterClockwise
  };
};

/// <summary>
/// The specific graphics API used by the <see cref="RHIGraphicsDevice"/>.
/// </summary>
struct EZ_RHI_DLL RHIGraphicsBackend // : byte
{
  using StorageType = ezUInt8;
  enum Enum
  {
    /// <summary>
    /// Direct3D 11.
    /// </summary>
    Direct3D11,
    /// <summary>
    /// Vulkan.
    /// </summary>
    Vulkan,
    /// <summary>
    /// OpenGL.
    /// </summary>
    OpenGL,
    /// <summary>
    /// Metal.
    /// </summary>
    Metal,
    /// <summary>
    /// OpenGL ES.
    /// </summary>
    OpenGLES,
    Default = Direct3D11
  };
};

/// <summary>
/// Identifies how a <see cref="RHIMappableResource"/> will be mapped into CPU address space.
/// </summary>
struct EZ_RHI_DLL RHIMapMode // : byte
{
  using StorageType = ezUInt8;
  enum Enum
  {
    /// <summary>
    /// A read-only resource mapping. The mapped data region is not writable, and cannot be used to transfer data into the
    /// graphics resource. NOTE: This mode can only be used on resources created with the Staging usage flag.
    /// </summary>
    Read,
    /// <summary>
    /// A write-only resource mapping. The mapped data region is writable, and will be transferred into the graphics resource
    /// when <see cref="RHIGraphicsDevice.Unmap(RHIMappableResource, ezUInt32)"/> is called. NOTE: upon mapping a buffer with this
    /// mode, the previous contents of the resource will be erased. This mode can only be used to entirely replace the
    /// contents of a resource.
    /// </summary>
    Write,
    /// <summary>
    /// A read-write resource mapping. The mapped data region is both readable and writable. NOTE: this mode can only be used
    /// on resources created with the Staging usage flag.
    /// </summary>
    ReadWrite,
    Default = Read
  };
};

/// <summary>
/// The format of data stored in a <see cref="RHITexture"/>.
/// Each name is a compound identifier, where each component denotes a name and a number of bits used to store that
/// component. The final component identifies the storage type of each component. "Float" identifies a signed, floating-point
/// type, UNorm identifies an unsigned integer type which is normalized, meaning it occupies the full space of the integer
/// type. The SRgb suffix for normalized integer formats indicates that the RGB components are stored in sRGB format.
/// </summary>
struct EZ_RHI_DLL RHIPixelFormat // : byte
{
  using StorageType = ezUInt8;
  enum Enum
  {
    /// <summary>
    /// RGBA component order. Each component is an 8-bit unsigned normalized integer.
    /// </summary>
    R8_G8_B8_A8_UNorm,
    /// <summary>
    /// BGRA component order. Each component is an 8-bit unsigned normalized integer.
    /// </summary>
    B8_G8_R8_A8_UNorm,
    /// <summary>
    /// Single-channel, 8-bit unsigned normalized integer.
    /// </summary>
    R8_UNorm,
    /// <summary>
    /// Single-channel, 16-bit unsigned normalized integer. Can be used as a depth format.
    /// </summary>
    R16_UNorm,
    /// <summary>
    /// RGBA component order. Each component is a 32-bit signed floating-point value.
    /// </summary>
    R32_G32_B32_A32_Float,
    /// <summary>
    /// Single-channel, 32-bit signed floating-point value. Can be used as a depth format.
    /// </summary>
    R32_Float,
    /// <summary>
    /// BC3 block compressed format.
    /// </summary>
    BC3_UNorm,
    /// <summary>
    /// A depth-stencil format where the depth is stored in a 24-bit unsigned normalized integer, and the stencil is stored
    /// in an 8-bit unsigned integer.
    /// </summary>
    D24_UNorm_S8_UInt,
    /// <summary>
    /// A depth-stencil format where the depth is stored in a 32-bit signed floating-point value, and the stencil is stored
    /// in an 8-bit unsigned integer.
    /// </summary>
    D32_Float_S8_UInt,
    /// <summary>
    /// RGBA component order. Each component is a 32-bit unsigned integer.
    /// </summary>
    R32_G32_B32_A32_UInt,
    /// <summary>
    /// RG component order. Each component is an 8-bit signed normalized integer.
    /// </summary>
    R8_G8_SNorm,
    /// <summary>
    /// BC1 block compressed format with no alpha.
    /// </summary>
    BC1_Rgb_UNorm,
    /// <summary>
    /// BC1 block compressed format with a single-bit alpha channel.
    /// </summary>
    BC1_Rgba_UNorm,
    /// <summary>
    /// BC2 block compressed format.
    /// </summary>
    BC2_UNorm,
    /// <summary>
    /// A 32-bit packed format. The 10-bit R component occupies bits 0..9, the 10-bit G component occupies bits 10..19,
    /// the 10-bit A component occupies 20..29, and the 2-bit A component occupies bits 30..31. Each value is an unsigned,
    /// normalized integer.
    /// </summary>
    R10_G10_B10_A2_UNorm,
    /// <summary>
    /// A 32-bit packed format. The 10-bit R component occupies bits 0..9, the 10-bit G component occupies bits 10..19,
    /// the 10-bit A component occupies 20..29, and the 2-bit A component occupies bits 30..31. Each value is an unsigned
    /// integer.
    /// </summary>
    R10_G10_B10_A2_UInt,
    /// <summary>
    /// A 32-bit packed format. The 11-bit R componnent occupies bits 0..10, the 11-bit G component occupies bits 11..21,
    /// and the 10-bit B component occupies bits 22..31. Each value is an unsigned floating point value.
    /// </summary>
    R11_G11_B10_Float,
    /// <summary>
    /// Single-channel, 8-bit signed normalized integer.
    /// </summary>
    R8_SNorm,
    /// <summary>
    /// Single-channel, 8-bit unsigned integer.
    /// </summary>
    R8_UInt,
    /// <summary>
    /// Single-channel, 8-bit signed integer.
    /// </summary>
    R8_SInt,
    /// <summary>
    /// Single-channel, 16-bit signed normalized integer.
    /// </summary>
    R16_SNorm,
    /// <summary>
    /// Single-channel, 16-bit unsigned integer.
    /// </summary>
    R16_UInt,
    /// <summary>
    /// Single-channel, 16-bit signed integer.
    /// </summary>
    R16_SInt,
    /// <summary>
    /// Single-channel, 16-bit signed floating-point value.
    /// </summary>
    R16_Float,
    /// <summary>
    /// Single-channel, 32-bit unsigned integer
    /// </summary>
    R32_UInt,
    /// <summary>
    /// Single-channel, 32-bit signed integer
    /// </summary>
    R32_SInt,
    /// <summary>
    /// RG component order. Each component is an 8-bit unsigned normalized integer.
    /// </summary>
    R8_G8_UNorm,
    /// <summary>
    /// RG component order. Each component is an 8-bit unsigned integer.
    /// </summary>
    R8_G8_UInt,
    /// <summary>
    /// RG component order. Each component is an 8-bit signed integer.
    /// </summary>
    R8_G8_SInt,
    /// <summary>
    /// RG component order. Each component is a 16-bit unsigned normalized integer.
    /// </summary>
    R16_G16_UNorm,
    /// <summary>
    /// RG component order. Each component is a 16-bit signed normalized integer.
    /// </summary>
    R16_G16_SNorm,
    /// <summary>
    /// RG component order. Each component is a 16-bit unsigned integer.
    /// </summary>
    R16_G16_UInt,
    /// <summary>
    /// RG component order. Each component is a 16-bit signed integer.
    /// </summary>
    R16_G16_SInt,
    /// <summary>
    /// RG component order. Each component is a 16-bit signed floating-point value.
    /// </summary>
    R16_G16_Float,
    /// <summary>
    /// RG component order. Each component is a 32-bit unsigned integer.
    /// </summary>
    R32_G32_UInt,
    /// <summary>
    /// RG component order. Each component is a 32-bit signed integer.
    /// </summary>
    R32_G32_SInt,
    /// <summary>
    /// RG component order. Each component is a 32-bit signed floating-point value.
    /// </summary>
    R32_G32_Float,
    /// <summary>
    /// RGBA component order. Each component is an 8-bit signed normalized integer.
    /// </summary>
    R8_G8_B8_A8_SNorm,
    /// <summary>
    /// RGBA component order. Each component is an 8-bit unsigned integer.
    /// </summary>
    R8_G8_B8_A8_UInt,
    /// <summary>
    /// RGBA component order. Each component is an 8-bit signed integer.
    /// </summary>
    R8_G8_B8_A8_SInt,
    /// <summary>
    /// RGBA component order. Each component is a 16-bit unsigned normalized integer.
    /// </summary>
    R16_G16_B16_A16_UNorm,
    /// <summary>
    /// RGBA component order. Each component is a 16-bit signed normalized integer.
    /// </summary>
    R16_G16_B16_A16_SNorm,
    /// <summary>
    /// RGBA component order. Each component is a 16-bit unsigned integer.
    /// </summary>
    R16_G16_B16_A16_UInt,
    /// <summary>
    /// RGBA component order. Each component is a 16-bit signed integer.
    /// </summary>
    R16_G16_B16_A16_SInt,
    /// <summary>
    /// RGBA component order. Each component is a 16-bit floating-point value.
    /// </summary>
    R16_G16_B16_A16_Float,
    /// <summary>
    /// RGBA component order. Each component is a 32-bit signed integer.
    /// </summary>
    R32_G32_B32_A32_SInt,
    /// <summary>
    /// A 64-bit, 4x4 block-compressed format storing unsigned normalized RGB data.
    /// </summary>
    ETC2_R8_G8_B8_UNorm,
    /// <summary>
    /// A 64-bit, 4x4 block-compressed format storing unsigned normalized RGB data, as well as 1 bit of alpha data.
    /// </summary>
    ETC2_R8_G8_B8_A1_UNorm,
    /// <summary>
    /// A 128-bit, 4x4 block-compressed format storing 64 bits of unsigned normalized RGB data, as well as 64 bits of alpha
    /// data.
    /// </summary>
    ETC2_R8_G8_B8_A8_UNorm,
    /// <summary>
    /// BC4 block compressed format, unsigned normalized values.
    /// </summary>
    BC4_UNorm,
    /// <summary>
    /// BC4 block compressed format, signed normalized values.
    /// </summary>
    BC4_SNorm,
    /// <summary>
    /// BC5 block compressed format, unsigned normalized values.
    /// </summary>
    BC5_UNorm,
    /// <summary>
    /// BC5 block compressed format, signed normalized values.
    /// </summary>
    BC5_SNorm,
    /// <summary>
    /// BC7 block compressed format.
    /// </summary>
    BC7_UNorm,
    /// <summary>
    /// RGBA component order. Each component is an 8-bit unsigned normalized integer.
    /// This is an sRGB format.
    /// </summary>
    R8_G8_B8_A8_UNorm_SRgb,
    /// <summary>
    /// BGRA component order. Each component is an 8-bit unsigned normalized integer.
    /// This is an sRGB format.
    /// </summary>
    B8_G8_R8_A8_UNorm_SRgb,
    /// <summary>
    /// BC1 block compressed format with no alpha.
    /// This is an sRGB format.
    /// </summary>
    BC1_Rgb_UNorm_SRgb,
    /// <summary>
    /// BC1 block compressed format with a single-bit alpha channel.
    /// This is an sRGB format.
    /// </summary>
    BC1_Rgba_UNorm_SRgb,
    /// <summary>
    /// BC2 block compressed format.
    /// This is an sRGB format.
    /// </summary>
    BC2_UNorm_SRgb,
    /// <summary>
    /// BC3 block compressed format.
    /// This is an sRGB format.
    /// </summary>
    BC3_UNorm_SRgb,
    /// <summary>
    /// BC7 block compressed format.
    /// This is an sRGB format.
    /// </summary>
    BC7_UNorm_SRgb,
    Default = R8_G8_B8_A8_UNorm
  };
};

/// <summary>
/// Indicates how the rasterizer will fill polygons.
/// </summary>
struct EZ_RHI_DLL RHIPolygonFillMode // : byte
{
  using StorageType = ezUInt8;
  enum Enum
  {
    /// <summary>
    /// Polygons are filled completely.
    /// </summary>
    Solid,
    /// <summary>
    /// Polygons are outlined in a "wireframe" style.
    /// </summary>
    Wireframe
  };
};

/// <summary>
/// Determines how a sequence of vertices is interepreted by the rasterizer.
/// </summary>
struct EZ_RHI_DLL RHIPrimitiveTopology // : byte
{
  using StorageType = ezUInt8;
  enum Enum
  {
    /// <summary>
    /// A list of isolated, 3-element triangles.
    /// </summary>
    TriangleList,
    /// <summary>
    /// A series of connected triangles.
    /// </summary>
    TriangleStrip,
    /// <summary>
    /// A series of isolated, 2-element line segments.
    /// </summary>
    LineList,
    /// <summary>
    /// A series of connected line segments.
    /// </summary>
    LineStrip,
    /// <summary>
    /// A series of isolated points.
    /// </summary>
    PointList,
    Default = TriangleList
  };
};

/// <summary>
/// Identifies a particular binding model used when connecting elements in a <see cref="RHIResourceLayout"/> with resources
/// defined in API-specific shader code.
/// </summary>
struct EZ_RHI_DLL RHIResourceBindingModel // : byte
{
  using StorageType = ezUInt8;
  enum Enum
  {
    /// <summary>
    /// The default binding model.
    /// </summary>
    Default = 0,
    /// <summary>
    /// An improved binding model which enables greater shader reuse and authoring flexibility.
    /// </summary>
    Improved = 1
  };
};

/// <summary>
/// The kind of a <see cref="BindableResource"/> object.
/// </summary>
struct EZ_RHI_DLL RHIResourceKind // : byte
{
  using StorageType = ezUInt8;
  enum Enum
  {
    /// <summary>
    /// A <see cref="RHIDeviceBuffer"/> accessed as a uniform buffer. A subset of a buffer can be bound using a
    /// <see cref="RHIDeviceBufferRange"/>.
    /// </summary>
    UniformBuffer,
    /// <summary>
    /// A <see cref="RHIDeviceBuffer"/> accessed as a read-only storage buffer. A subset of a buffer can be bound using a
    /// <see cref="RHIDeviceBufferRange"/>.
    /// </summary>
    StructuredBufferReadOnly,
    /// <summary>
    /// A <see cref="RHIDeviceBuffer"/> accessed as a read-write storage buffer. A subset of a buffer can be bound using a
    /// <see cref="RHIDeviceBufferRange"/>.
    /// </summary>
    StructuredBufferReadWrite,
    /// <summary>
    /// A read-only <see cref="RHITexture"/>, accessed through a Texture or <see cref="RHITextureView"/>.
    /// <remarks>Binding a <see cref="RHITexture"/> to a resource slot expecting a TextureReadWrite is equivalent to binding a
    /// <see cref="RHITextureView"/> that covers the full mip and array layer range, with the original Texture's
    /// <see cref="RHIPixelFormat"/>.</remarks>
    /// </summary>
    TextureReadOnly,
    /// <summary>
    /// A read-write <see cref="RHITexture"/>, accessed through a Texture or <see cref="RHITextureView"/>.
    /// </summary>
    /// <remarks>Binding a <see cref="RHITexture"/> to a resource slot expecting a TextureReadWrite is equivalent to binding a
    /// <see cref="RHITextureView"/> that covers the full mip and array layer range, with the original Texture's
    /// <see cref="RHIPixelFormat"/>.</remarks>
    TextureReadWrite,
    /// <summary>
    /// A <see cref="RHISampler"/>.
    /// </summary>
    Sampler,
    Default = UniformBuffer
  };
};

/// <summary>
/// Miscellaneous options for an element in a <see cref="RHIResourceLayout"/>.
/// </summary>
struct EZ_RHI_DLL RHIResourceLayoutElementOptions // : int
{
  using StorageType = ezInt32;
  enum Enum
  {
    /// <summary>
    /// No special options.
    /// </summary>
    None,
    /// <summary>
    /// Can be applied to a buffer type resource (<see cref="RHIResourceKind.StructuredBufferReadOnly"/>,
    /// <see cref="RHIResourceKind.StructuredBufferReadWrite"/>, or <see cref="RHIResourceKind.UniformBuffer"/>), allowing it to be
    /// bound with a dynamic offset using <see cref="RHICommandList.SetGraphicsResourceSet(ezUInt32, RHIResourceSet, ezDynamicArray<ezUInt32>)"/>.
    /// Offsets specified this way must be a multiple of <see cref="RHIGraphicsDevice.UniformBufferMinOffsetAlignment"/> or
    /// <see cref="RHIGraphicsDevice.StructuredBufferMinOffsetAlignment"/>.
    /// </summary>
    DynamicBinding = 1 << 0,
    Default = None
  };

  struct Bits
  {
  };
};
EZ_DECLARE_FLAGS_OPERATORS(RHIResourceLayoutElementOptions);

/// <summary>
/// An addressing mode for texture coordinates.
/// </summary>
struct EZ_RHI_DLL RHISamplerAddressMode // : byte
{
  using StorageType = ezUInt8;
  enum Enum
  {
    /// <summary>
    /// Texture coordinates are wrapped upon overflow.
    /// </summary>
    Wrap,
    /// <summary>
    /// Texture coordinates are mirrored upon overflow.
    /// </summary>
    Mirror,
    /// <summary>
    /// Texture coordinates are clamped to the maximum or minimum values upon overflow.
    /// </summary>
    Clamp,
    /// <summary>
    /// Texture coordinates that overflow return the predefined border color defined in
    /// <see cref="SamplerDescription.BorderColor"/>.
    /// </summary>
    Border,
    Default = Wrap
  };
};

/// <summary>
/// A constant color that is sampled when <see cref="SamplerAddressMode.Border"/> is used.
/// </summary>
struct EZ_RHI_DLL RHISamplerBorderColor // : byte
{
  using StorageType = ezUInt8;
  enum Enum
  {
    /// <summary>
    /// Transparent Black (0, 0, 0, 0)
    /// </summary>
    TransparentBlack,
    /// <summary>
    /// Opaque Black (0, 0, 0, 1)
    /// </summary>
    OpaqueBlack,
    /// <summary>
    /// Opaque White (1, 1, 1, 1)
    /// </summary>
    OpaqueWhite,
    Default = TransparentBlack
  };
};

/// <summary>
/// Determines how texture values are sampled from a texture.
/// </summary>
struct EZ_RHI_DLL RHISamplerFilter // : byte
{
  using StorageType = ezUInt8;
  enum Enum
  {
    /// <summary>
    /// Point sampling is used for minification, magnification, and mip-level sampling.
    /// </summary>
    MinPoint_MagPoint_MipPoint,
    /// <summary>
    /// Point sampling is used for minification and magnification; linear interpolation is used for mip-level sampling.
    /// </summary>
    MinPoint_MagPoint_MipLinear,
    /// <summary>
    /// Point sampling is used for minification and mip-level sampling; linear interpolation is used for mip-level sampling.
    /// </summary>
    MinPoint_MagLinear_MipPoint,
    /// <summary>
    /// Point sampling is used for minification; linear interpolation is used for magnification and mip-level sampling.
    /// </summary>
    MinPoint_MagLinear_MipLinear,
    /// <summary>
    /// Linear interpolation is used for minifcation; point sampling is used for magnification and mip-level sampling.
    /// </summary>
    MinLinear_MagPoint_MipPoint,
    /// <summary>
    /// Linear interpolation is used for minification and mip-level sampling; point sampling is used for magnification.
    /// </summary>
    MinLinear_MagPoint_MipLinear,
    /// <summary>
    /// Linear interpolation is used for minification and magnification, and point sampling is used for mip-level sampling.
    /// </summary>
    MinLinear_MagLinear_MipPoint,
    /// <summary>
    /// Linear interpolation is used for minification, magnification, and mip-level sampling.
    /// </summary>
    MinLinear_MagLinear_MipLinear,
    /// <summary>
    /// Anisotropic filtering is used. The maximum anisotropy is controlled by
    /// <see cref="SamplerDescription.MaximumAnisotropy"/>.
    /// </summary>
    Anisotropic,
    Default = MinPoint_MagPoint_MipPoint
  };
};

/// <summary>
/// The data type of a shader constant.
/// </summary>
struct EZ_RHI_DLL RHIShaderConstantType // : ezUInt32
{
  using StorageType = ezUInt32;
  enum Enum
  {
    /// <summary>
    /// A boolean.
    /// </summary>
    Bool,
    /// <summary>
    /// A 16-bit unsigned integer.
    /// </summary>
    UInt16,
    /// <summary>
    /// A 16-bit signed integer.
    /// </summary>
    Int16,
    /// <summary>
    /// A 32-bit unsigned integer.
    /// </summary>
    UInt32,
    /// <summary>
    /// A 32-bit signed integer.
    /// </summary>
    Int32,
    /// <summary>
    /// A 64-bit unsigned integer.
    /// </summary>
    UInt64,
    /// <summary>
    /// A 64-bit signed integer.
    /// </summary>
    Int64,
    /// <summary>
    /// A 32-bit floating-point value.
    /// </summary>
    Float,
    /// <summary>
    /// A 64-bit floating-point value.
    /// </summary>
    Double,
    Default = Bool
  };
};

/// <summary>
/// A bitmask representing a set of shader stages.
/// </summary>
struct EZ_RHI_DLL RHIShaderStages //  : byte
{
  using StorageType = ezUInt8;
  enum Enum
  {
    /// <summary>
    /// No stages.
    /// </summary>
    None = 0,
    /// <summary>
    /// The vertex shader stage.
    /// </summary>
    Vertex = 1 << 0,
    /// <summary>
    /// The geometry shader stage.
    /// </summary>
    Geometry = 1 << 1,
    /// <summary>
    /// The tessellation control (or hull) shader stage.
    /// </summary>
    TessellationControl = 1 << 2,
    /// <summary>
    /// The tessellation evaluation (or domain) shader stage.
    /// </summary>
    TessellationEvaluation = 1 << 3,
    /// <summary>
    /// The fragment (or pixel) shader stage.
    /// </summary>
    Fragment = 1 << 4,
    /// <summary>
    /// The compute shader stage.
    /// </summary>
    Compute = 1 << 5,
    Default = None
  };

  struct Bits
  {
  };
};
EZ_DECLARE_FLAGS_OPERATORS(RHIShaderStages);

/// <summary>
/// Identifies an action taken on samples that pass or fail the stencil test.
/// </summary>
struct EZ_RHI_DLL RHIStencilOperation // : byte
{
  using StorageType = ezUInt8;
  enum Enum
  {
    /// <summary>
    /// Keep the existing value.
    /// </summary>
    Keep,
    /// <summary>
    /// Sets the value to 0.
    /// </summary>
    Zero,
    /// <summary>
    /// Replaces the existing value with <see cref="RHIDepthStencilStateDescription.StencilReference"/>.
    /// </summary>
    Replace,
    /// <summary>
    /// Increments the existing value and clamps it to the maximum representable unsigned value.
    /// </summary>
    IncrementAndClamp,
    /// <summary>
    /// Decrements the existing value and clamps it to 0.
    /// </summary>
    DecrementAndClamp,
    /// <summary>
    /// Bitwise-inverts the existing value.
    /// </summary>
    Invert,
    /// <summary>
    /// Increments the existing value and wraps it to 0 when it exceeds the maximum representable unsigned value.
    /// </summary>
    IncrementAndWrap,
    /// <summary>
    /// Decrements the existing value and wraps it to the maximum representable unsigned value if it would be reduced below
    /// 0.
    /// </summary>
    DecrementAndWrap,
    Default = Keep
  };
};

/// <summary>
/// Describes the number of samples to use in a <see cref="RHITexture"/>.
/// </summary>
struct EZ_RHI_DLL RHITextureSampleCount // : byte
{
  using StorageType = ezUInt8;
  enum Enum
  {
    /// <summary>
    /// 1 sample (no multi-sampling).
    /// </summary>
    Count1,
    /// <summary>
    /// 2 Samples.
    /// </summary>
    Count2,
    /// <summary>
    /// 4 Samples.
    /// </summary>
    Count4,
    /// <summary>
    /// 8 Samples.
    /// </summary>
    Count8,
    /// <summary>
    /// 16 Samples.
    /// </summary>
    Count16,
    /// <summary>
    /// 32 Samples.
    /// </summary>
    Count32,
    Default = Count1
  };
};

/// <summary>
/// Identifies a particular type of Texture.
/// </summary>
struct EZ_RHI_DLL RHITextureType // : ezUInt32
{
  using StorageType = ezUInt32;
  enum Enum
  {
    /// <summary>
    /// A one-dimensional Texture.
    /// </summary>
    Texture1D,
    /// <summary>
    /// A two-dimensional Texture.
    /// </summary>
    Texture2D,
    /// <summary>
    /// A three-dimensional Texture.
    /// </summary>
    Texture3D,
    Default = Texture1D
  };
};

/// <summary>
/// A bitmask indicating how a <see cref="RHITexture"/> is permitted to be used.
/// </summary>
struct EZ_RHI_DLL RHITextureUsage // : byte
{
  using StorageType = ezUInt8;
  enum Enum
  {
    /// <summary>
    /// The Texture can be used as the target of a read-only <see cref="RHITextureView"/>, and can be accessed from a shader.
    /// </summary>
    Sampled = 1 << 0,
    /// <summary>
    /// The Texture can be used as the target of a read-write <see cref="RHITextureView"/>, and can be accessed from a shader.
    /// </summary>
    Storage = 1 << 1,
    /// <summary>
    /// The Texture can be used as the color target of a <see cref="RHIFramebuffer"/>.
    /// </summary>
    RenderTarget = 1 << 2,
    /// <summary>
    /// The Texture can be used as the depth target of a <see cref="RHIFramebuffer"/>.
    /// </summary>
    DepthStencil = 1 << 3,
    /// <summary>
    /// The Texture is a two-dimensional cubemap.
    /// </summary>
    Cubemap = 1 << 4,
    /// <summary>
    /// The Texture is used as a read-write staging resource for uploading Texture data.
    /// With this flag, a Texture can be mapped using the <see cref="RHIGraphicsDevice.Map(RHIMappableResource, RHIMapMode, ezUInt32)"/>
    /// method.
    /// </summary>
    Staging = 1 << 5,
    /// <summary>
    /// The Texture supports automatic generation of mipmaps through <see cref="RHICommandList.GenerateMipmaps(RHITexture)"/>.
    /// </summary>
    GenerateMipmaps = 1 << 6,
    Default = Sampled
  };

  struct Bits
  {
  };
};
EZ_DECLARE_FLAGS_OPERATORS(RHITextureUsage);

/// <summary>
/// The format of an individual vertex element.
/// </summary>
struct EZ_RHI_DLL RHIVertexElementFormat // : byte
{
  using StorageType = ezUInt8;
  enum Enum
  {
    /// <summary>
    /// One 32-bit floating point value.
    /// </summary>
    Float1,
    /// <summary>
    /// Two 32-bit floating point values.
    /// </summary>
    Float2,
    /// <summary>
    /// Three 32-bit floating point values.
    /// </summary>
    Float3,
    /// <summary>
    /// Four 32-bit floating point values.
    /// </summary>
    Float4,
    /// <summary>
    /// Two 8-bit unsigned normalized integers.
    /// </summary>
    Byte2_Norm,
    /// <summary>
    /// Two 8-bit unisgned integers.
    /// </summary>
    Byte2,
    /// <summary>
    /// Four 8-bit unsigned normalized integers.
    /// </summary>
    Byte4_Norm,
    /// <summary>
    /// Four 8-bit unsigned integers.
    /// </summary>
    Byte4,
    /// <summary>
    /// Two 8-bit signed normalized integers.
    /// </summary>
    SByte2_Norm,
    /// <summary>
    /// Two 8-bit signed integers.
    /// </summary>
    SByte2,
    /// <summary>
    /// Four 8-bit signed normalized integers.
    /// </summary>
    SByte4_Norm,
    /// <summary>
    /// Four 8-bit signed integers.
    /// </summary>
    SByte4,
    /// <summary>
    /// Two 16-bit unsigned normalized integers.
    /// </summary>
    UShort2_Norm,
    /// <summary>
    /// Two 16-bit unsigned integers.
    /// </summary>
    UShort2,
    /// <summary>
    /// Four 16-bit unsigned normalized integers.
    /// </summary>
    UShort4_Norm,
    /// <summary>
    /// Four 16-bit unsigned integers.
    /// </summary>
    UShort4,
    /// <summary>
    /// Two 16-bit signed normalized integers.
    /// </summary>
    Short2_Norm,
    /// <summary>
    /// Two 16-bit signed integers.
    /// </summary>
    Short2,
    /// <summary>
    /// Four 16-bit signed normalized integers.
    /// </summary>
    Short4_Norm,
    /// <summary>
    /// Four 16-bit signed integers.
    /// </summary>
    Short4,
    /// <summary>
    /// One 32-bit unsigned integer.
    /// </summary>
    UInt1,
    /// <summary>
    /// Two 32-bit unsigned integers.
    /// </summary>
    UInt2,
    /// <summary>
    /// Three 32-bit unsigned integers.
    /// </summary>
    UInt3,
    /// <summary>
    /// Four 32-bit unsigned integers.
    /// </summary>
    UInt4,
    /// <summary>
    /// One 32-bit signed integer.
    /// </summary>
    Int1,
    /// <summary>
    /// Two 32-bit signed integers.
    /// </summary>
    Int2,
    /// <summary>
    /// Three 32-bit signed integers.
    /// </summary>
    Int3,
    /// <summary>
    /// Four 32-bit signed integers.
    /// </summary>
    Int4,
    /// <summary>
    /// One 16-bit floating point value.
    /// </summary>
    Half1,
    /// <summary>
    /// Two 16-bit floating point values.
    /// </summary>
    Half2,
    /// <summary>
    /// Four 16-bit floating point values.
    /// </summary>
    Half4,
    Default = Float1
  };
};

/// <summary>
/// The type of a vertex element, describing how the element is interpreted.
/// NOTE: This enumeration is only meaningful for the Direct3D 11 backend.
/// When using SPIRV to cross-compile a vertex shader to HLSL, all vertex elements will
/// use <see cref="RHIVertexElementSemantic.TextureCoordinate"/>.
/// </summary>
struct EZ_RHI_DLL RHIVertexElementSemantic // : byte
{
  using StorageType = ezUInt8;
  enum Enum
  {
    /// <summary>
    /// A position.
    /// </summary>
    Position,
    /// <summary>
    /// A normal direction.
    /// </summary>
    Normal,
    /// <summary>
    /// A texture coordinate.
    /// </summary>
    TextureCoordinate,
    /// <summary>
    /// A color.
    /// </summary>
    Color,
    Default = Position
  };
};

/// <summary>
/// A structure describing the format expected by indirect dispatch commands contained in an indirect <see cref="RHIDeviceBuffer"/>.
/// </summary>
struct EZ_RHI_DLL IndirectDispatchArguments
{
  /// <summary>
  /// The X group count, as if passed to the <see cref="RHICommandList.Dispatch(ezUInt32, ezUInt32, ezUInt32)"/> method.
  /// </summary>
  ezUInt32 GroupCountX;
  /// <summary>
  /// The Y group count, as if passed to the <see cref="RHICommandList.Dispatch(ezUInt32, ezUInt32, ezUInt32)"/> method.
  /// </summary>
  ezUInt32 GroupCountY;
  /// <summary>
  /// The Z group count, as if passed to the <see cref="RHICommandList.Dispatch(ezUInt32, ezUInt32, ezUInt32)"/> method.
  /// </summary>
  ezUInt32 GroupCountZ;
};

/// <summary>
/// A structure describing the format expected by indirect draw commands contained in an indirect <see cref="DeviceBuffer"/>.
/// </summary>
struct EZ_RHI_DLL IndirectDrawArguments
{
  /// <summary>
  /// The number of vertices to draw.
  /// </summary>
  ezUInt32 VertexCount;
  /// <summary>
  /// The number of instances to draw.
  /// </summary>
  ezUInt32 InstanceCount;
  /// <summary>
  /// The first vertex to draw. Subsequent vertices are incremented by 1.
  /// </summary>
  ezUInt32 FirstVertex;
  /// <summary>
  /// The first instance to draw. Subsequent instances are incrmented by 1.
  /// </summary>
  ezUInt32 FirstInstance;
};

/// <summary>
/// A structure describing the format expected by indirect, indexed draw commands contained in an indirect
/// <see cref="RHIDeviceBuffer"/>.
/// </summary>
struct EZ_RHI_DLL IndirectDrawIndexedArguments
{
  /// <summary>
  /// The number of indices to use in the indexed draw.
  /// </summary>
  ezUInt32 IndexCount;
  /// <summary>
  /// The number of instances to draw.
  /// </summary>
  ezUInt32 InstanceCount;
  /// <summary>
  /// The value which is used as the start of the indices used for the draw.
  /// </summary>
  ezUInt32 FirstIndex;
  /// <summary>
  /// An offset which is added to each vertex element referenced by the index <see cref="DeviceBuffer"/>.
  /// </summary>
  ezInt32 VertexOffset;
  /// <summary>
  /// The first instance to draw. Subsequent instances (if InstanceCount > 1) are incremented by 1.
  /// </summary>
  ezUInt32 FirstInstance;
};


/// <summary>
/// Describes the properties that are supported for a particular combination of <see cref="PixelFormat"/>,
/// <see cref="TextureType"/>, and <see cref="TextureUsage"/> by a <see cref="GraphicsDevice"/>.
/// See <see cref="GraphicsDevice.GetPixelFormatSupport(PixelFormat, TextureType, TextureUsage, out PixelFormatProperties)"/>.
/// </summary>
struct RHIPixelFormatProperties
{
  /// <summary>
  /// The maximum supported width.
  /// </summary>
  ezUInt32 GetMaxWidth() const
  {
    return MaxWidth;
  }

  /// <summary>
  /// The maximum supported height.
  /// </summary>
  ezUInt32 GetMaxHeight() const
  {
    return MaxHeight;
  }

  /// <summary>
  /// The maximum supported depth.
  /// </summary>
  ezUInt32 GetMaxDepth() const
  {
    return MaxDepth;
  }

  /// <summary>
  /// The maximum supported number of mipmap levels.
  /// </summary>
  ezUInt32 GetMaxMipLevels() const
  {
    return MaxMipLevels;
  }

  /// <summary>
  /// The maximum supported number of array layers.
  /// </summary>
  ezUInt32 GetMaxArrayLayers() const
  {
    return MaxArrayLayers;
  }

  /// <summary>
  /// Gets a value indicating whether or not the given <see cref="TextureSampleCount"/> is supported.
  /// </summary>
  /// <param name="count">The <see cref="TextureSampleCount"/> to query.</param>
  /// <returns>True if the sample count is supported; false otherwise.</returns>
  bool IsSampleCountSupported(ezEnum<RHITextureSampleCount> count)
  {
    ezInt32 bit = (ezInt32)count;
    return (SampleCounts & (1 << bit)) != 0;
  }

  RHIPixelFormatProperties() = default;

  RHIPixelFormatProperties(
    ezUInt32 maxWidth,
    ezUInt32 maxHeight,
    ezUInt32 maxDepth,
    ezUInt32 maxMipLevels,
    ezUInt32 maxArrayLayers,
    ezUInt32 sampleCounts)
  {
    MaxWidth = maxWidth;
    MaxHeight = maxHeight;
    MaxDepth = maxDepth;
    MaxMipLevels = maxMipLevels;
    MaxArrayLayers = maxArrayLayers;
    SampleCounts = sampleCounts;
  }

private:
  ezUInt32 SampleCounts = 0;
  ezUInt32 MaxWidth = 0;
  ezUInt32 MaxHeight = 0;
  ezUInt32 MaxDepth = 0;
  ezUInt32 MaxMipLevels = 0;
  ezUInt32 MaxArrayLayers = 0;
};

/// <summary>
/// Enumerates the optional features supported by a given <see cref="GraphicsDevice"/>.
/// </summary>
class RHIGraphicsDeviceFeatures
{
public:
  /// <summary>
  /// Indicates whether Compute Shaders can be used.
  /// </summary>
  bool ComputeShaderSupported() const { return ComputeShader; }

  /// <summary>
  /// Indicates whether Geometry Shaders can be used.
  /// </summary>
  bool GeometryShaderSupported() const { return GeometryShader; }

  /// <summary>
  /// Indicates whether Tessellation Shaders can be used.
  /// </summary>
  bool TessellationShadersSupported() const { return TessellationShaders; }

  /// <summary>
  /// Indicates whether multiple independent viewports can be set simultaneously.
  /// If this is not supported, then only the first Viewport index will be used for all render outputs.
  /// </summary>
  bool MultipleViewportsSupported() const { return MultipleViewports; }

  /// <summary>
  /// Indicates whether <see cref="SamplerDescription.LodBias"/> can be non-zero.
  /// If false, it is an error to attempt to use a non-zero bias value.
  /// </summary>
  bool SamplerLodBiasSupported() const { return SamplerLodBias; }

  /// <summary>
  /// Indicates whether a non-zero "vertexStart" value can be used in
  /// <see cref="CommandList.Draw(ezUInt32, ezUInt32, ezUInt32, ezUInt32)"/> and
  /// <see cref="CommandList.DrawIndexed(ezUInt32, ezUInt32, ezUInt32, int, ezUInt32)"/>.
  /// </summary>
  bool DrawBaseVertexSupported() const { return DrawBaseVertex; }

  /// <summary>
  /// Indicates whether a non-zero "instanceStart" value can be used in
  /// <see cref="CommandList.Draw(ezUInt32, ezUInt32, ezUInt32, ezUInt32)"/> and
  /// <see cref="CommandList.DrawIndexed(ezUInt32, ezUInt32, ezUInt32, int, ezUInt32)"/>.
  /// </summary>
  bool DrawBaseInstanceSupported() const { return DrawBaseInstance; }

  /// <summary>
  /// Indicates whether indirect draw commands can be issued.
  /// </summary>
  bool DrawIndirectSupported() const { return DrawIndirect; }

  /// <summary>
  /// Indicates whether indirect draw structures stored in an Indirect DeviceBuffer can contain
  /// a non-zero FirstInstance value.
  /// </summary>
  bool DrawIndirectBaseInstanceSupported() const { return DrawIndirectBaseInstance; }

  /// <summary>
  /// Indicates whether <see cref="PolygonFillMode.Wireframe"/> is supported.
  /// </summary>
  bool FillModeWireframeSupported() const { return FillModeWireframe; }

  /// <summary>
  /// Indicates whether <see cref="SamplerFilter.Anisotropic"/> is supported.
  /// </summary>
  bool SamplerAnisotropySupported() const { return SamplerAnisotropy; }

  /// <summary>
  /// Indicates whether <see cref="RasterizerStateDescription.DepthClipEnabled"/> can be set to false.
  /// </summary>
  bool DepthClipDisableSupported() const { return DepthClipDisable; }

  /// <summary>
  /// Indicates whether a <see cref="Texture"/> can be created with <see cref="TextureType.Texture1D"/>.
  /// </summary>
  bool Texture1DSupported() const { return Texture1D; }

  /// <summary>
  /// Indicates whether a <see cref="BlendStateDescription"/> can be used which has multiple different
  /// <see cref="BlendAttachmentDescription"/> values for each attachment. If false, all attachments must have the same
  /// blend state.
  /// </summary>
  bool IndependentBlendSupported() const { return IndependentBlend; }

  /// <summary>
  /// Indicates whether <see cref="BufferUsage.StructuredBufferReadOnly"/> and
  /// <see cref="BufferUsage.StructuredBufferReadWrite"/> can be used. If false, structured buffers cannot be created.
  /// </summary>
  bool StructuredBufferSupported() const { return StructuredBuffer; }

  /// <summary>
  /// Indicates whether a <see cref="TextureView"/> can be created which does not view the full set of mip levels and array
  /// layers contained in its target Texture, or uses a different <see cref="PixelFormat"/> from the underlying Texture.
  /// </summary>
  bool SubsetTextureViewSupported() const { return SubsetTextureView; }

  /// <summary>
  /// Indicates whether <see cref="CommandList"/> instances created with this device support the
  /// <see cref="CommandList.PushDebugGroup(string)"/>, <see cref="CommandList.PopDebugGroup"/>, and
  /// <see cref="CommandList.InsertDebugMarker(string)"/> methods. If not, these methods will have no effect.
  /// </summary>
  bool CommandListDebugMarkersSupported() const { return CommandListDebugMarkers; }

  /// <summary>
  /// Indicates whether uniform and structured buffers can be bound with an offset and a size. If false, buffer resources
  /// must be bound with their full range.
  /// </summary>
  bool BufferRangeBindingSupported() const { return BufferRangeBinding; }

  /// <summary>
  /// Indicates whether 64-bit floating point integers can be used in shaders.
  /// </summary>
  bool ShaderFloat64Supported() const { return ShaderFloat64; }

  RHIGraphicsDeviceFeatures() {}

  RHIGraphicsDeviceFeatures(bool computeShader,
    bool geometryShader,
    bool tessellationShaders,
    bool multipleViewports,
    bool samplerLodBias,
    bool drawBaseVertex,
    bool drawBaseInstance,
    bool drawIndirect,
    bool drawIndirectBaseInstance,
    bool fillModeWireframe,
    bool samplerAnisotropy,
    bool depthClipDisable,
    bool texture1D,
    bool independentBlend,
    bool structuredBuffer,
    bool subsetTextureView,
    bool commandListDebugMarkers,
    bool bufferRangeBinding,
    bool shaderFloat64)
  {

    ComputeShader = computeShader;
    GeometryShader = geometryShader;
    TessellationShaders = tessellationShaders;
    MultipleViewports = multipleViewports;
    SamplerLodBias = samplerLodBias;
    DrawBaseVertex = drawBaseVertex;
    DrawBaseInstance = drawBaseInstance;
    DrawIndirect = drawIndirect;
    DrawIndirectBaseInstance = drawIndirectBaseInstance;
    FillModeWireframe = fillModeWireframe;
    SamplerAnisotropy = samplerAnisotropy;
    DepthClipDisable = depthClipDisable;
    Texture1D = texture1D;
    IndependentBlend = independentBlend;
    StructuredBuffer = structuredBuffer;
    SubsetTextureView = subsetTextureView;
    CommandListDebugMarkers = commandListDebugMarkers;
    BufferRangeBinding = bufferRangeBinding;
    ShaderFloat64 = shaderFloat64;
  }

private:
  bool ComputeShader = false;
  bool GeometryShader = false;
  bool TessellationShaders = false;
  bool MultipleViewports = false;
  bool SamplerLodBias = false;
  bool DrawBaseVertex = false;
  bool DrawBaseInstance = false;
  bool DrawIndirect = false;
  bool DrawIndirectBaseInstance = false;
  bool FillModeWireframe = false;
  bool SamplerAnisotropy = false;
  bool DepthClipDisable = false;
  bool Texture1D = false;
  bool IndependentBlend = false;
  bool StructuredBuffer = false;
  bool SubsetTextureView = false;
  bool CommandListDebugMarkers = false;
  bool BufferRangeBinding = false;
  bool ShaderFloat64 = false;
};

/// <summary>
/// Defines an integer rectangle (Left, Top, Right, Bottom)
/// </summary>
struct RHIRawRect
{
  /// <summary>
  /// The left position.
  /// </summary>
  ezInt32 Left = 0;

  /// <summary>
  /// The top position.
  /// </summary>
  ezInt32 Top = 0;

  /// <summary>
  /// The right position
  /// </summary>
  ezInt32 Right = 0;

  /// <summary>
  /// The bottom position.
  /// </summary>
  ezInt32 Bottom = 0;

  RHIRawRect(ezInt32 left, ezInt32 top, ezInt32 right, ezInt32 bottom)
  {
    Left = left;
    Top = top;
    Right = right;
    Bottom = bottom;
  }

  /// <summary>
  /// Performs an implicit conversion from <see cre ="RawRect"/> to <see cref="System.Drawing.Rectangle" />.
  /// </summary>
  /// <param name="value">The value to convert.</param>
  /// <returns>The result of the conversion.</returns>
  //static implicit operator Rectangle(RawRect value) = > Rectangle.FromLTRB(value.Left, value.Top, value.Right, value.Bottom);

  /// <summary>
  /// Performs an implicit conversion from <see cre ="Rectangle"/> to <see cref="RawRect" />.
  /// </summary>
  /// <param name="value">The value to convert.</param>
  /// <returns>The result of the conversion.</returns>
  //static implicit operator RawRect(Rectangle value) = > new RawRect(value.Left, value.Top, value.Right, value.Bottom);
};
