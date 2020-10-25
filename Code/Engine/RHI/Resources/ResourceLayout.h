#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Resources/DeviceResource.h>

/// <summary>
/// Miscellaneous options for an element in a <see cref="RHIResourceLayout"/>.
/// </summary>
struct EZ_RHI_DLL RHIResourceLayoutElementOptions // : int
{
  using StorageType = ezUInt32;
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
    DynamicBinding = 1 << 0
  };

  struct Bits
  {
  };
};
EZ_DECLARE_FLAGS_OPERATORS(RHIResourceLayoutElementOptions);

/// <summary>
/// Describes an individual resource element in a <see cref="RHIResourceLayout"/>.
/// </summary>
struct EZ_RHI_DLL RHIResourceLayoutElementDescription : public ezHashableStruct<RHIResourceLayoutElementDescription>
{
  /// <summary>
  /// The name of the element.
  /// </summary>
  ezString Name;

  /// <summary>
  /// The kind of resource.
  /// </summary>
  ezEnum<RHIResourceKind> Kind;

  /// <summary>
  /// The <see cref="RHIShaderStages"/> in which this element is used.
  /// </summary>
  ezBitflags<RHIShaderStages> Stages;

  /// <summary>
  /// Miscellaneous resource options for this element.
  /// </summary>
  ezBitflags<RHIResourceLayoutElementOptions> Options;

  /// <summary>
  /// Constructs a new ResourceLayoutElementDescription.
  /// </summary>
  /// <param name="name">The name of the element.</param>
  /// <param name="kind">The kind of resource.</param>
  /// <param name="stages">The <see cref="RHIShaderStages"/> in which this element is used.</param>
  RHIResourceLayoutElementDescription(ezString name, ezEnum<RHIResourceKind> kind, ezBitflags<RHIShaderStages> stages)
  {
    Name = name;
    Kind = kind;
    Stages = stages;
    Options = RHIResourceLayoutElementOptions::None;
  }

  /// <summary>
  /// Constructs a new ResourceLayoutElementDescription.
  /// </summary>
  /// <param name="name">The name of the element.</param>
  /// <param name="kind">The kind of resource.</param>
  /// <param name="stages">The <see cref="RHIShaderStages"/> in which this element is used.</param>
  /// <param name="options">Miscellaneous resource options for this element.</param>
  RHIResourceLayoutElementDescription(
    ezString name,
    ezEnum<RHIResourceKind> kind,
    ezBitflags<RHIShaderStages> stages,
    ezBitflags<RHIResourceLayoutElementOptions> options)
  {
    Name = name;
    Kind = kind;
    Stages = stages;
    Options = options;
  }

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements and all array elements are equal; false otherswise.</returns>
  bool operator==(const RHIResourceLayoutElementDescription& other) const
  {
    return Name.IsEqual(other.Name) && Kind == other.Kind && Stages == other.Stages && Options == other.Options;
  }
};

/// <summary>
/// Describes the layout of <see cref="RHIResourseFlags::Bindable"/> objects for a <see cref="RHIPipeline"/>.
/// </summary>
struct EZ_RHI_DLL RHIResourceLayoutDescription : public ezHashableStruct<RHIResourceLayoutDescription>
{
  /// <summary>
  /// An array of <see cref="RHIResourceLayoutElementDescription"/> objects, describing the properties of each resource
  /// element in the <see cref="RHIResourceLayout"/>.
  /// </summary>
  ezDynamicArray<RHIResourceLayoutElementDescription> Elements;

  RHIResourceLayoutDescription() = default;

  /// <summary>
  /// Constructs a new ResourceLayoutDescription.
  /// </summary>
  /// <param name="elements">An array of <see cref="RHIResourceLayoutElementDescription"/> objects, describing the properties
  /// of each resource element in the <see cref="RHIResourceLayout"/>.</param>
  RHIResourceLayoutDescription(ezDynamicArray<RHIResourceLayoutElementDescription>& elements)
  {
    Elements = elements;
  }

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements and all array elements are equal; false otherswise.</returns>
  bool operator==(const RHIResourceLayoutDescription& other) const
  {
    return Elements == other.Elements;
  }
};

/// <summary>
/// A device resource which describes the layout and kind of <see cref="RHIBindableResource"/> objects available
/// to a shader set.
/// See <see cref="RHIResourceLayoutDescription"/>.
/// </summary>
class RHIResourceLayout : public RHIDeviceResource
{
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
  RHIResourceLayoutDescription Description;
  ezUInt32 DynamicBufferCount;
#endif

  RHIResourceLayout(const RHIResourceLayoutDescription& description)
  {
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    Description = description;
    for (RHIResourceLayoutElementDescription element : description.Elements)
    {
      if ((element.Options & RHIResourceLayoutElementOptions::DynamicBinding) != 0)
      {
        DynamicBufferCount += 1;
      }
    }
#endif
  }

public:
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
  const RHIResourceLayoutDescription& GetDescription() const
  {
    return Description;
  }

  ezUInt32 GetDynamicBufferCount() const
  {
    return DynamicBufferCount;
  }
#endif
};
