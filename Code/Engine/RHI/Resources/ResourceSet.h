#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>
#include <optional>

#include <RHI/Resources/DeviceResource.h>
#include <RHI/Resources/ResourceLayout.h>

/// <summary>
/// Describes a <see cref="ResourceSet"/>, for creation using a <see cref="ResourceFactory"/>.
/// </summary>
struct EZ_RHI_DLL RHIResourceSetDescription : public ezHashableStruct<RHIResourceSetDescription>
{
  EZ_DECLARE_POD_TYPE();

  /// <summary>
  /// The <see cref="ResourceLayout"/> describing the number and kind of resources used.
  /// </summary>
  RHIResourceLayout* Layout;
  /// <summary>
  /// An array of <see cref="BindableResource"/> objects.
  /// The number and type of resources must match those specified in the <see cref="ResourceLayout"/>.
  /// </summary>
  ezDynamicArray<RHIDeviceResource*> BoundResources;

  /// <summary>
  /// Constructs a new ResourceSetDescription.
  /// </summary>
  /// <param name="layout">The <see cref="ResourceLayout"/> describing the number and kind of resources used.</param>
  /// <param name="boundResources">An array of <see cref="BindableResource"/> objects.
  /// The number and type of resources must match those specified in the <see cref="ResourceLayout"/>.</param>
  RHIResourceSetDescription(RHIResourceLayout* layout, const ezDynamicArray<RHIDeviceResource*>& boundResources)
  {
    Layout = layout;
    BoundResources = boundResources;
  }

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements are equal; false otherswise.</returns>
  virtual bool operator==(const RHIResourceSetDescription& other) const
  {
    return Layout == other.Layout && BoundResources == other.BoundResources;
  }
};

/// <summary>
/// A device resource used to bind a particular set of <see cref="BindableResource"/> objects to a <see cref="CommandList"/>.
/// See <see cref="ResourceSetDescription"/>.
/// </summary>
class EZ_RHI_DLL RHIResourceSet : public RHIDeviceResource
{
public:
  RHIResourceSet(const RHIResourceSetDescription& description)
  {
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    Layout = description.Layout;
    Resources = description.BoundResources;
#endif
  }

#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
  RHIResourceLayout* GetLayout() const
  {
    return Layout;
  }

  ezDynamicArray<RHIDeviceResource*> GetResources() const
  {
    return Resources;
  }
#endif

private:
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
  RHIResourceLayout* Layout;
  ezDynamicArray<RHIDeviceResource*> Resources;
#endif
};

struct RHIBoundResourceSetInfo : public ezHashableStruct<RHIBoundResourceSetInfo>
{
  RHIResourceSet* Set = nullptr;
  ezHybridArray<ezUInt32, 5> Offsets;

  RHIBoundResourceSetInfo(RHIResourceSet* set, ezDynamicArray<ezUInt32> offsets)
  {
    Set = set;
    Offsets.SetCountUninitialized(offsets.GetCount());
    Offsets.GetArrayPtr().CopyFrom(offsets.GetArrayPtr());
  }

  bool Equals(RHIResourceSet* set, ezUInt32 offsetsCount, ezDynamicArray<ezUInt32> offsets) const
  {
    if (Set != set || offsetsCount != Offsets.GetCount())
      return false;

    if (Offsets.GetCount() != offsets.GetCount())
      return false;

    for (ezUInt32 i = 0; i < Offsets.GetCount(); i++)
    {
      if (Offsets[i] != offsets[i])
        return false;
    }

    return true;
  }

  bool Equals(const RHIBoundResourceSetInfo& other) const
  {
    if (Set != other.Set || Offsets.GetCount() != Offsets.GetCount())
      return false;

    if (Offsets.GetCount() != other.Offsets.GetCount())
      return false;

    for (ezUInt32 i = 0; i < Offsets.GetCount(); i++)
    {
      if (Offsets[i] != other.Offsets[i])
        return false;
    }

    return true;
  }

  bool operator==(const RHIBoundResourceSetInfo& other) const
  {
    return this->Equals(other);
  }
};
