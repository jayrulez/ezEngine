#include <RHI/Shader/Shader.h>
#include <RHIDX12/BindingSetLayout/DXBindingSetLayout.h>
#include <RHIDX12/Device/DXDevice.h>
#include <RHIDX12/Pipeline/DXRayTracingPipeline.h>
#include <RHIDX12/Program/DXProgram.h>
#include <RHIDX12/Utilities/DXUtility.h>
#include <RHIDX12/View/DXView.h>
#include <d3d12shader.h>
#include <directx/d3d12.h>
#include <directx/d3dx12.h>

DXRayTracingPipeline::DXRayTracingPipeline(DXDevice& device, const RayTracingPipelineDesc& desc)
  : m_device(device)
  , m_desc(desc)
{
  decltype(auto) shaders = m_desc.program->GetShaders();
  decltype(auto) dx_layout = m_desc.layout->As<DXBindingSetLayout>();
  m_root_signature = dx_layout.GetRootSignature();

  CD3DX12_STATE_OBJECT_DESC subobjects(D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE);

  decltype(auto) entry_points = m_desc.program->GetEntryPoints();
  for (const auto& shader : shaders)
  {
    decltype(auto) library = subobjects.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
    decltype(auto) blob = shader->GetBlob();
    D3D12_SHADER_BYTECODE byte = {blob.data(), blob.size()};
    library->SetDXILLibrary(&byte);
    for (const auto& entry_point : shader->GetReflection()->GetEntryPoints())
    {
      uint64_t shader_id = shader->GetId(entry_point.name);
      ezString shader_name = entry_point.name;
      if (m_shader_names.Contains(shader_name))
      {
        ezStringBuilder renamed(shader_name, "_renamed_");
        renamed.AppendFormat("{}", shader_id);
        ezString new_shader_name = GenerateUniqueName(renamed);
        library->DefineExport(ezStringWChar(new_shader_name).GetData(), ezStringWChar(shader_name).GetData());
        shader_name = new_shader_name;
      }
      else
      {
        library->DefineExport(ezStringWChar(shader_name).GetData());
      }
      m_shader_names.Insert(shader_name);
      m_shader_ids[shader_id] = shader_name;
    }
  }

  size_t hit_group_count = 0;
  for (size_t i = 0; i < m_desc.groups.size(); ++i)
  {
    if (m_desc.groups[i].type == RayTracingShaderGroupType::kGeneral)
    {
      m_group_names[i] = m_shader_ids[m_desc.groups[i].general];
      continue;
    }
    decltype(auto) hit_group = subobjects.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
    ezStringBuilder hitGroupName("hit_group_");
    hitGroupName.AppendFormat("{}", hit_group_count++);
    ezString name = GenerateUniqueName(hitGroupName);
    hit_group->SetHitGroupExport(ezStringWChar(name).GetData());
    switch (m_desc.groups[i].type)
    {
      case RayTracingShaderGroupType::kTrianglesHitGroup:
        if (m_desc.groups[i].any_hit)
        {
          hit_group->SetAnyHitShaderImport(ezStringWChar(m_shader_ids[m_desc.groups[i].any_hit]).GetData());
        }
        if (m_desc.groups[i].closest_hit)
        {
          hit_group->SetClosestHitShaderImport(ezStringWChar(m_shader_ids[m_desc.groups[i].closest_hit]).GetData());
        }
        break;
      case RayTracingShaderGroupType::kProceduralHitGroup:
        if (m_desc.groups[i].intersection)
        {
          hit_group->SetIntersectionShaderImport(ezStringWChar(m_shader_ids[m_desc.groups[i].intersection]).GetData());
        }
        break;
    }
    m_group_names[i] = name;
  }

  decltype(auto) global_root_signature = subobjects.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
  global_root_signature->SetRootSignature(m_root_signature.Get());

  decltype(auto) shader_config = subobjects.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();

  uint32_t max_payload_size = 0;
  uint32_t max_attribute_size = 0;
  for (size_t i = 0; i < entry_points.size(); ++i)
  {
    max_payload_size = ezMath::Max(max_payload_size, entry_points[i].payload_size);
    max_attribute_size = ezMath::Max(max_payload_size, entry_points[i].attribute_size);
  }
  shader_config->Config(max_payload_size, max_attribute_size);

  decltype(auto) pipeline_config = subobjects.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
  pipeline_config->Config(1);

  ComPtr<ID3D12Device5> device5;
  m_device.GetDevice().As(&device5);
  EZ_ASSERT_ALWAYS(device5->CreateStateObject(subobjects, IID_PPV_ARGS(&m_pipeline_state)) == S_OK, "");
  m_pipeline_state.As(&m_state_ojbect_props);
}

ezString DXRayTracingPipeline::GenerateUniqueName(ezString name)
{
  ezStringBuilder builder(name);
  static uint64_t id = 0;
  while (m_shader_names.Contains(builder))
  {
    builder.AppendFormat("_{}", ++id);
  }
  return builder.GetData();
}

PipelineType DXRayTracingPipeline::GetPipelineType() const
{
  return PipelineType::kRayTracing;
}

const RayTracingPipelineDesc& DXRayTracingPipeline::GetDesc() const
{
  return m_desc;
}

const ComPtr<ID3D12StateObject>& DXRayTracingPipeline::GetPipeline() const
{
  return m_pipeline_state;
}

const ComPtr<ID3D12RootSignature>& DXRayTracingPipeline::GetRootSignature() const
{
  return m_root_signature;
}

std::vector<uint8_t> DXRayTracingPipeline::GetRayTracingShaderGroupHandles(uint32_t first_group, uint32_t group_count) const
{
  std::vector<uint8_t> shader_handles_storage(group_count * m_device.GetShaderGroupHandleSize());
  for (uint32_t i = 0; i < group_count; ++i)
  {
    memcpy(shader_handles_storage.data() + i * m_device.GetShaderGroupHandleSize(), m_state_ojbect_props->GetShaderIdentifier(ezStringWChar((*m_group_names.GetValue(i + first_group))).GetData()), m_device.GetShaderGroupHandleSize());
  }
  return shader_handles_storage;
}
