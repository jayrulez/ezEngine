#include <RHIShaderCompilerHLSL/SPIRVReflection.h>

ezRHIShaderKind ConvertShaderKind(spv::ExecutionModel execution_model)
{
  switch (execution_model)
  {
    case spv::ExecutionModel::ExecutionModelVertex:
      return ezRHIShaderKind::Vertex;
    case spv::ExecutionModel::ExecutionModelFragment:
      return ezRHIShaderKind::Pixel;
    case spv::ExecutionModel::ExecutionModelGeometry:
      return ezRHIShaderKind::Geometry;
    case spv::ExecutionModel::ExecutionModelGLCompute:
      return ezRHIShaderKind::Compute;
    case spv::ExecutionModel::ExecutionModelRayGenerationNV:
      return ezRHIShaderKind::RayGeneration;
    case spv::ExecutionModel::ExecutionModelIntersectionNV:
      return ezRHIShaderKind::Intersection;
    case spv::ExecutionModel::ExecutionModelAnyHitNV:
      return ezRHIShaderKind::AnyHit;
    case spv::ExecutionModel::ExecutionModelClosestHitNV:
      return ezRHIShaderKind::ClosestHit;
    case spv::ExecutionModel::ExecutionModelMissNV:
      return ezRHIShaderKind::Miss;
    case spv::ExecutionModel::ExecutionModelCallableNV:
      return ezRHIShaderKind::Callable;
    case spv::ExecutionModel::ExecutionModelTaskNV:
      return ezRHIShaderKind::Amplification;
    case spv::ExecutionModel::ExecutionModelMeshNV:
      return ezRHIShaderKind::Mesh;
  }
  assert(false);
  return ezRHIShaderKind::Unknown;
}

ezDynamicArray<ezRHIInputParameterDescription> ParseInputParameters(const spirv_cross::Compiler& compiler)
{
  spirv_cross::ShaderResources resources = compiler.get_shader_resources();
  ezDynamicArray<ezRHIInputParameterDescription> inputParameters;
  for (const auto& resource : resources.stage_inputs)
  {
    decltype(auto) input = inputParameters.ExpandAndGetRef();
    input.m_Location = compiler.get_decoration(resource.id, spv::DecorationLocation);
    input.m_SemanticName = compiler.get_decoration_string(resource.id, spv::DecorationHlslSemanticGOOGLE).data();
    if (!input.m_SemanticName.IsEmpty() && input.m_SemanticName.GetLast(1).GetCharacter() == '0')
    {
      input.m_SemanticName = input.m_SemanticName.GetSubString(0, input.m_SemanticName.GetCharacterCount() - 1);
    }
    decltype(auto) type = compiler.get_type(resource.base_type_id);
    if (type.basetype == spirv_cross::SPIRType::Float)
    {
      if (type.vecsize == 1)
      {
        input.m_Format = ezRHIResourceFormat::R32_FLOAT; // gli::format::FORMAT_R32_SFLOAT_PACK32;
      }
      else if (type.vecsize == 2)
      {
        input.m_Format = ezRHIResourceFormat::R32G32_FLOAT; //gli::format::FORMAT_RG32_SFLOAT_PACK32;
      }
      else if (type.vecsize == 3)
      {
        input.m_Format = ezRHIResourceFormat::R32G32B32_FLOAT; //gli::format::FORMAT_RGB32_SFLOAT_PACK32;
      }
      else if (type.vecsize == 4)
      {
        input.m_Format = ezRHIResourceFormat::R32G32B32A32_FLOAT; //gli::format::FORMAT_RGBA32_SFLOAT_PACK32;
      }
    }
    else if (type.basetype == spirv_cross::SPIRType::UInt)
    {
      if (type.vecsize == 1)
      {
        input.m_Format = ezRHIResourceFormat::R32_UINT; //gli::format::FORMAT_R32_UINT_PACK32;
      }
      else if (type.vecsize == 2)
      {
        input.m_Format = ezRHIResourceFormat::R32G32_UINT; //gli::format::FORMAT_RG32_UINT_PACK32;
      }
      else if (type.vecsize == 3)
      {
        input.m_Format = ezRHIResourceFormat::R32G32B32_UINT; //gli::format::FORMAT_RGB32_UINT_PACK32;
      }
      else if (type.vecsize == 4)
      {
        input.m_Format = ezRHIResourceFormat::R32G32B32A32_UINT; //gli::format::FORMAT_RGBA32_UINT_PACK32;
      }
    }
    else if (type.basetype == spirv_cross::SPIRType::Int)
    {
      if (type.vecsize == 1)
      {
        input.m_Format = ezRHIResourceFormat::R32_SINT; //gli::format::FORMAT_R32_SINT_PACK32;
      }
      else if (type.vecsize == 2)
      {
        input.m_Format = ezRHIResourceFormat::R32G32_SINT; //gli::format::FORMAT_RG32_SINT_PACK32;
      }
      else if (type.vecsize == 3)
      {
        input.m_Format = ezRHIResourceFormat::R32G32B32_SINT; //gli::format::FORMAT_RGB32_SINT_PACK32;
      }
      else if (type.vecsize == 4)
      {
        input.m_Format = ezRHIResourceFormat::R32G32B32A32_SINT; //gli::format::FORMAT_RGBA32_SINT_PACK32;
      }
    }
  }
  return inputParameters;
}

ezDynamicArray<ezRHIOutputParameterDescription> ParseOutputParameters(const spirv_cross::Compiler& compiler)
{
  spirv_cross::ShaderResources resources = compiler.get_shader_resources();
  ezDynamicArray<ezRHIOutputParameterDescription> outputParameters;
  for (const auto& resource : resources.stage_outputs)
  {
    decltype(auto) output = outputParameters.ExpandAndGetRef();
    output.m_Slot = compiler.get_decoration(resource.id, spv::DecorationLocation);
  }
  return outputParameters;
}

bool IsBufferDimension(spv::Dim dimension)
{
  switch (dimension)
  {
    case spv::Dim::DimBuffer:
      return true;
    case spv::Dim::Dim1D:
    case spv::Dim::Dim2D:
    case spv::Dim::Dim3D:
    case spv::Dim::DimCube:
      return false;
    default:
      assert(false);
      return false;
  }
}

ezRHIViewType GetViewType(const spirv_cross::Compiler& compiler, const spirv_cross::SPIRType& type, ezUInt32 resource_id)
{
  switch (type.basetype)
  {
    case spirv_cross::SPIRType::AccelerationStructure:
    {
      return ezRHIViewType::AccelerationStructure;
    }
    case spirv_cross::SPIRType::SampledImage:
    case spirv_cross::SPIRType::Image:
    {
      bool is_readonly = (type.image.sampled != 2);
      if (IsBufferDimension(type.image.dim))
      {
        if (is_readonly)
          return ezRHIViewType::Buffer;
        else
          return ezRHIViewType::RWBuffer;
      }
      else
      {
        if (is_readonly)
          return ezRHIViewType::Texture;
        else
          return ezRHIViewType::RWTexture;
      }
    }
    case spirv_cross::SPIRType::Sampler:
    {
      return ezRHIViewType::Sampler;
    }
    case spirv_cross::SPIRType::Struct:
    {
      if (type.storage == spv::StorageClassStorageBuffer)
      {
        spirv_cross::Bitset flags = compiler.get_buffer_block_flags(resource_id);
        bool is_readonly = flags.get(spv::DecorationNonWritable);
        if (is_readonly)
        {
          return ezRHIViewType::StructuredBuffer;
        }
        else
        {
          return ezRHIViewType::RWStructuredBuffer;
        }
      }
      else if (type.storage == spv::StorageClassPushConstant || type.storage == spv::StorageClassUniform)
      {
        return ezRHIViewType::ConstantBuffer;
      }
      assert(false);
      return ezRHIViewType::Unknown;
    }
    default:
      assert(false);
      return ezRHIViewType::Unknown;
  }
}

ezRHIViewDimension GetDimension(spv::Dim dim, const spirv_cross::SPIRType& resource_type)
{
  switch (dim)
  {
    case spv::Dim::Dim1D:
    {
      if (resource_type.image.arrayed)
        return ezRHIViewDimension::Texture1DArray;
      else
        return ezRHIViewDimension::Texture1D;
    }
    case spv::Dim::Dim2D:
    {
      if (resource_type.image.arrayed)
        return ezRHIViewDimension::Texture2DArray;
      else
        return ezRHIViewDimension::Texture2D;
    }
    case spv::Dim::Dim3D:
    {
      return ezRHIViewDimension::Texture3D;
    }
    case spv::Dim::DimCube:
    {
      if (resource_type.image.arrayed)
        return ezRHIViewDimension::TextureCubeArray;
      else
        return ezRHIViewDimension::TextureCube;
    }
    case spv::Dim::DimBuffer:
    {
      return ezRHIViewDimension::Buffer;
    }
    default:
      assert(false);
      return ezRHIViewDimension::Unknown;
  }
}

ezRHIViewDimension GetViewDimension(const spirv_cross::SPIRType& resource_type)
{
  if (resource_type.basetype == spirv_cross::SPIRType::BaseType::Image)
  {
    return GetDimension(resource_type.image.dim, resource_type);
  }
  else if (resource_type.basetype == spirv_cross::SPIRType::BaseType::Struct)
  {
    return ezRHIViewDimension::Buffer;
  }
  else
  {
    return ezRHIViewDimension::Unknown;
  }
}

ezRHIReturnType GetReturnType(const spirv_cross::CompilerHLSL& compiler, const spirv_cross::SPIRType& resource_type)
{
  if (resource_type.basetype == spirv_cross::SPIRType::BaseType::Image)
  {
    decltype(auto) image_type = compiler.get_type(resource_type.image.type);
    switch (image_type.basetype)
    {
      case spirv_cross::SPIRType::BaseType::Float:
        return ezRHIReturnType::Float;
      case spirv_cross::SPIRType::BaseType::UInt:
        return ezRHIReturnType::Uint;
      case spirv_cross::SPIRType::BaseType::Int:
        return ezRHIReturnType::Int;
      case spirv_cross::SPIRType::BaseType::Double:
        return ezRHIReturnType::Double;
    }
    assert(false);
  }
  return ezRHIReturnType::Unknown;
}

ezRHIResourceBindingDescription GetBindingDesc(const spirv_cross::CompilerHLSL& compiler, const spirv_cross::Resource& resource)
{
  ezRHIResourceBindingDescription desc = {};
  decltype(auto) type = compiler.get_type(resource.type_id);
  desc.m_Name = compiler.get_name(resource.id).data();
  desc.m_Type = GetViewType(compiler, type, resource.id);
  desc.m_Slot = compiler.get_decoration(resource.id, spv::DecorationBinding);
  desc.m_Space = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
  desc.m_Count = 1;
  if (!type.array.empty() && type.array.front() == 0)
  {
    desc.m_Count = std::numeric_limits<ezUInt32>::max();
  }
  desc.m_Dimension = GetViewDimension(type);
  desc.m_ReturnType = GetReturnType(compiler, type);
  switch (desc.m_Type)
  {
    case ezRHIViewType::StructuredBuffer:
    case ezRHIViewType::RWStructuredBuffer:
    {
      bool is_block = compiler.get_decoration_bitset(type.self).get(spv::DecorationBlock) ||
                      compiler.get_decoration_bitset(type.self).get(spv::DecorationBufferBlock);
      bool is_sized_block = is_block && (compiler.get_storage_class(resource.id) == spv::StorageClassUniform ||
                                          compiler.get_storage_class(resource.id) == spv::StorageClassUniformConstant ||
                                          compiler.get_storage_class(resource.id) == spv::StorageClassStorageBuffer);
      EZ_ASSERT_ALWAYS(is_sized_block, "");
      decltype(auto) base_type = compiler.get_type(resource.base_type_id);
      desc.StructureStride = (ezUInt32)(compiler.get_declared_struct_size_runtime_array(base_type, 1) - compiler.get_declared_struct_size_runtime_array(base_type, 0));
      EZ_ASSERT_ALWAYS(desc.StructureStride, "");
      break;
    }
  }
  return desc;
}

ezRHIVariableLayout GetBufferMemberLayout(const spirv_cross::CompilerHLSL& compiler, const spirv_cross::TypeID& type_id)
{
  decltype(auto) type = compiler.get_type(type_id);
  ezRHIVariableLayout layout = {};
  layout.m_Columns = type.vecsize;
  layout.m_Rows = type.columns;
  if (!type.array.empty())
  {
    assert(type.array.size() == 1);
    layout.m_Elements = type.array.front();
  }
  switch (type.basetype)
  {
    case spirv_cross::SPIRType::BaseType::Float:
      layout.m_Type = ezRHIVariableType::Float;
      break;
    case spirv_cross::SPIRType::BaseType::Int:
      layout.m_Type = ezRHIVariableType::Int;
      break;
    case spirv_cross::SPIRType::BaseType::UInt:
      layout.m_Type = ezRHIVariableType::Uint;
      break;
    case spirv_cross::SPIRType::BaseType::Boolean:
      layout.m_Type = ezRHIVariableType::Bool;
      break;
    default:
      EZ_ASSERT_NOT_IMPLEMENTED;
      break;
  }
  return layout;
}

ezRHIVariableLayout GetBufferLayout(ezRHIViewType view_type, const spirv_cross::CompilerHLSL& compiler, const spirv_cross::Resource& resource)
{
  if (view_type != ezRHIViewType::ConstantBuffer)
  {
    return {};
  }

  ezRHIVariableLayout layout = {};
  decltype(auto) type = compiler.get_type(resource.base_type_id);
  layout.m_Name = compiler.get_name(resource.id).data();
  layout.m_Size = (ezUInt32)compiler.get_declared_struct_size(type);
  assert(type.basetype == spirv_cross::SPIRType::BaseType::Struct);
  for (ezUInt32 i = 0; i < (ezUInt32)type.member_types.size(); ++i)
  {
    layout.m_Members.PushBack(GetBufferMemberLayout(compiler, type.member_types[i]));
    auto& member = layout.m_Members.PeekBack();
    member.m_Name = compiler.get_member_name(resource.base_type_id, i).data();
    member.m_Offset = compiler.type_struct_member_offset(type, i);
    member.m_Size = (ezUInt32)compiler.get_declared_struct_member_size(type, i);
  }
  return layout;
}

void ParseBindings(const spirv_cross::CompilerHLSL& compiler, ezDynamicArray<ezRHIResourceBindingDescription>& bindings, ezDynamicArray<ezRHIVariableLayout>& layouts)
{
  spirv_cross::ShaderResources resources = compiler.get_shader_resources();
  auto enumerate_resources = [&](const spirv_cross::SmallVector<spirv_cross::Resource>& resources) {
    for (const auto& resource : resources)
    {
      bindings.PushBack(GetBindingDesc(compiler, resource));
      layouts.PushBack(GetBufferLayout(bindings.PeekBack().m_Type, compiler, resource));
    }
  };
  enumerate_resources(resources.uniform_buffers);
  enumerate_resources(resources.storage_buffers);
  enumerate_resources(resources.storage_images);
  enumerate_resources(resources.separate_images);
  enumerate_resources(resources.separate_samplers);
  enumerate_resources(resources.atomic_counters);
  enumerate_resources(resources.acceleration_structures);
}

ezRHISPIRVReflection::ezRHISPIRVReflection(const void* data, ezUInt32 size)
{
  m_Blob.SetCount(size / sizeof(ezUInt32));
  ezMemoryUtils::Copy(m_Blob.GetData(), reinterpret_cast<const ezUInt32*>(data), m_Blob.GetCount());

  //spirv_cross::CompilerHLSL compiler(m_Blob);
  spirv_cross::CompilerHLSL compiler(m_Blob.GetData(), m_Blob.GetCount());
  auto entry_points = compiler.get_entry_points_and_stages();
  for (const auto& entry_point : entry_points)
  {
    ezRHIEntryPoint ep;
    ep.m_Name = entry_point.name.data();
    ep.m_Kind = ConvertShaderKind(entry_point.execution_model);
    m_EntryPoints.PushBack(std::move(ep));
  }
  ParseBindings(compiler, m_Bindings, m_Layouts);
  m_InputParameters = ParseInputParameters(compiler);
  m_OutputParameters = ParseOutputParameters(compiler);
}

const ezDynamicArray<ezRHIEntryPoint>& ezRHISPIRVReflection::GetEntryPoints() const
{
  return m_EntryPoints;
}

const ezDynamicArray<ezRHIResourceBindingDescription>& ezRHISPIRVReflection::GetBindings() const
{
  return m_Bindings;
}

const ezDynamicArray<ezRHIVariableLayout>& ezRHISPIRVReflection::GetVariableLayouts() const
{
  return m_Layouts;
}

const ezDynamicArray<ezRHIInputParameterDescription>& ezRHISPIRVReflection::GetInputParameters() const
{
  return m_InputParameters;
}

const ezDynamicArray<ezRHIOutputParameterDescription>& ezRHISPIRVReflection::GetOutputParameters() const
{
  return m_OutputParameters;
}

const ezRHIShaderFeatureInfo& ezRHISPIRVReflection::GetShaderFeatureInfo() const
{
  return m_ShaderFeatureInfo;
}
