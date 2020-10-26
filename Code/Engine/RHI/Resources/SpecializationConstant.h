#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <Foundation/Algorithm/HashableStruct.h>

/// <summary>
/// Describes a single shader specialization constant. Used to substitute new values into Shaders when constructing a
/// <see cref="RHIPipeline"/>.
/// </summary>
struct EZ_RHI_DLL RHISpecializationConstant : public ezHashableStruct<RHISpecializationConstant>
{
  /// <summary>
  /// The constant variable ID, as defined in the <see cref="RHIShader"/>.
  /// </summary>
  ezUInt32 ID = 0;

  /// <summary>
  /// The type of data stored in this instance. Must be a scalar numeric type.
  /// </summary>
  ezEnum<RHIShaderConstantType> Type;

  /// <summary>
  /// An 8-byte block storing the contents of the specialization value. This is treated as an untyped buffer and is
  /// interepreted according to <see cref="Type"/>.
  /// </summary>
  ezUInt64 Data = 0;

  /// <summary>
  /// Constructs a new <see cref="SpecializationConstant"/>.
  /// </summary>
  /// <param name="id">The constant variable ID, as defined in the <see cref="Shader"/>.</param>
  /// <param name="type">The type of data stored in this instance. Must be a scalar numeric type.</param>
  /// <param name="data">An 8-byte block storing the contents of the specialization value. This is treated as an untyped
  /// buffer and is interepreted according to <see cref="Type"/>.</param>
  RHISpecializationConstant(ezUInt32 id, ezEnum<RHIShaderConstantType> type, ezUInt64 data)
  {
    ID = id;
    Type = type;
    Data = data;
  }

  /// <summary>
  /// Constructs a new <see cref="SpecializationConstant"/> for a boolean.
  /// </summary>
  /// <param name="id">The constant variable ID.</param>
  /// <param name="value">The constant value.</param>
  RHISpecializationConstant(ezUInt32 id, bool value)
    : RHISpecializationConstant(id, RHIShaderConstantType::Bool, Store(value ? static_cast<unsigned char>(1u) : static_cast<unsigned char>(0u)))
  {
  }
  /// <summary>
  /// Constructs a new <see cref="SpecializationConstant"/> for a 16-bit unsigned integer.
  /// </summary>
  /// <param name="id">The constant variable ID.</param>
  /// <param name="value">The constant value.</param>
  RHISpecializationConstant(ezUInt32 id, ezUInt16 value)
    : RHISpecializationConstant(id, RHIShaderConstantType::UInt16, Store(value))
  {
  }
  /// <summary>
  /// Constructs a new <see cref="SpecializationConstant"/> for a 16-bit signed integer.
  /// </summary>
  /// <param name="id">The constant variable ID.</param>
  /// <param name="value">The constant value.</param>
  RHISpecializationConstant(ezUInt32 id, ezInt16 value)
    : RHISpecializationConstant(id, RHIShaderConstantType::Int16, Store(value))
  {
  }
  /// <summary>
  /// Constructs a new <see cref="SpecializationConstant"/> for a 32-bit unsigned integer.
  /// </summary>
  /// <param name="id">The constant variable ID.</param>
  /// <param name="value">The constant value.</param>
  RHISpecializationConstant(ezUInt32 id, ezUInt32 value)
    : RHISpecializationConstant(id, RHIShaderConstantType::UInt32, Store(value))
  {
  }
  /// <summary>
  /// Constructs a new <see cref="SpecializationConstant"/> for a 32-bit signed integer.
  /// </summary>
  /// <param name="id">The constant variable ID.</param>
  /// <param name="value">The constant value.</param>
  RHISpecializationConstant(ezUInt32 id, ezInt32 value)
    : RHISpecializationConstant(id, RHIShaderConstantType::Int32, Store(value))
  {
  }
  /// <summary>
  /// Constructs a new <see cref="SpecializationConstant"/> for a 64-bit unsigned integer.
  /// </summary>
  /// <param name="id">The constant variable ID.</param>
  /// <param name="value">The constant value.</param>
  RHISpecializationConstant(ezUInt32 id, ezUInt64 value)
    : RHISpecializationConstant(id, RHIShaderConstantType::UInt64, Store(value))
  {
  }
  /// <summary>
  /// Constructs a new <see cref="SpecializationConstant"/> for a 64-bit signed integer.
  /// </summary>
  /// <param name="id">The constant variable ID.</param>
  /// <param name="value">The constant value.</param>
  RHISpecializationConstant(ezUInt32 id, ezInt64 value)
    : RHISpecializationConstant(id, RHIShaderConstantType::Int64, Store(value))
  {
  }
  /// <summary>
  /// Constructs a new <see cref="SpecializationConstant"/> for a 32-bit floating-point value.
  /// </summary>
  /// <param name="id">The constant variable ID.</param>
  /// <param name="value">The constant value.</param>
  RHISpecializationConstant(ezUInt32 id, float value)
    : RHISpecializationConstant(id, RHIShaderConstantType::Float, Store(value))
  {
  }
  /// <summary>
  /// Constructs a new <see cref="SpecializationConstant"/> for a 64-bit floating-point value.
  /// </summary>
  /// <param name="id">The constant variable ID.</param>
  /// <param name="value">The constant value.</param>
  RHISpecializationConstant(ezUInt32 id, double value)
    : RHISpecializationConstant(id, RHIShaderConstantType::Double, Store(value))
  {
  }

  template <typename T>
  static ezUInt64 Store(T value)
  {
    ezUInt64 data;

    data = static_cast<ezUInt64>(value);

    return data;
  }

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements are equal; false otherswise.</returns>
  bool operator==(const RHISpecializationConstant& other) const
  {
    return ID == other.ID && Type == other.Type && Data == other.Data;
  }
};