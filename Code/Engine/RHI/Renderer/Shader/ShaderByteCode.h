
#pragma once

#include <Foundation/Containers/DynamicArray.h>
#include <Foundation/Types/RefCounted.h>
#include <RHI/Renderer/Basics.h>

/// \brief This class wraps shader byte code storage.
/// Since byte code can have different requirements for alignment, padding etc. this class manages it.
/// Also since byte code is shared between multiple shaders (e.g. same vertex shaders for different pixel shaders)
/// the instances of the byte codes are reference counted.
class EZ_RHI_DLL ezRHIShaderByteCode : public ezRefCounted
{
public:
  ezRHIShaderByteCode();

  ezRHIShaderByteCode(const ezArrayPtr<const ezUInt8>& pByteCode);

  inline const void* GetByteCode() const;

  inline ezUInt32 GetSize() const;

  inline bool IsValid() const;

protected:
  void CopyFrom(const ezArrayPtr<const ezUInt8>& pByteCode);

  ezDynamicArray<ezUInt8> m_Source;
};

#include <RHI/Renderer/Shader/Implementation/ShaderByteCode_inl.h>
