#include <RHIVulkan/Adapter/VKAdapter.h>
#include <RHIVulkan/BindingSet/VKBindingSet.h>
#include <RHIVulkan/CommandList/VKCommandList.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/Framebuffer/VKFramebuffer.h>
#include <RHIVulkan/Pipeline/VKComputePipeline.h>
#include <RHIVulkan/Pipeline/VKGraphicsPipeline.h>
#include <RHIVulkan/Pipeline/VKRayTracingPipeline.h>
#include <RHIVulkan/QueryHeap/VKQueryHeap.h>
#include <RHIVulkan/Resource/VKResource.h>
#include <RHIVulkan/Utilities/VKUtility.h>
#include <RHIVulkan/View/VKView.h>

VKCommandList::VKCommandList(VKDevice& device, CommandListType type)
  : m_device(device)
{
  VkCommandBufferAllocateInfo cmd_buf_alloc_info = {};
  cmd_buf_alloc_info.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  cmd_buf_alloc_info.commandPool = device.GetCmdPool(type);
  cmd_buf_alloc_info.commandBufferCount = 1;
  cmd_buf_alloc_info.level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  std::vector<VkCommandBuffer> cmd_bufs(cmd_buf_alloc_info.commandBufferCount);
  vkAllocateCommandBuffers(device.GetDevice(), &cmd_buf_alloc_info, cmd_bufs.data());
  m_command_list = std::move(cmd_bufs.front());

  VkCommandBufferBeginInfo begin_info = {};
  begin_info.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  vkBeginCommandBuffer(m_command_list, &begin_info);
}

void VKCommandList::Reset()
{
  Close();
  VkCommandBufferBeginInfo begin_info = {};
  begin_info.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  vkBeginCommandBuffer(m_command_list, &begin_info);
  m_closed = false;
  m_state.reset();
  m_binding_set.reset();
}

void VKCommandList::Close()
{
  if (!m_closed)
  {
    vkEndCommandBuffer(m_command_list);
    m_closed = true;
  }
}

VkPipelineBindPoint GetPipelineBindPoint(PipelineType type)
{
  switch (type)
  {
    case PipelineType::kGraphics:
      return VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS;
    case PipelineType::kCompute:
      return VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_COMPUTE;
    case PipelineType::kRayTracing:
      return VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;
  }
  assert(false);
  return {};
}

void VKCommandList::BindPipeline(const std::shared_ptr<Pipeline>& state)
{
  if (state == m_state)
    return;
  m_state = std::static_pointer_cast<VKPipeline>(state);

  vkCmdBindPipeline(m_command_list, GetPipelineBindPoint(m_state->GetPipelineType()), m_state->GetPipeline());
}

void VKCommandList::BindBindingSet(const std::shared_ptr<BindingSet>& binding_set)
{
  if (binding_set == m_binding_set)
    return;
  m_binding_set = binding_set;
  decltype(auto) vk_binding_set = binding_set->As<VKBindingSet>();
  decltype(auto) descriptor_sets = vk_binding_set.GetDescriptorSets();
  if (descriptor_sets.empty())
    return;

  vkCmdBindDescriptorSets(m_command_list, GetPipelineBindPoint(m_state->GetPipelineType()), m_state->GetPipelineLayout(), 0, (ezUInt32)descriptor_sets.size(), descriptor_sets.data(), 0, nullptr);
}

void VKCommandList::BeginRenderPass(const std::shared_ptr<RenderPass>& render_pass, const std::shared_ptr<Framebuffer>& framebuffer, const ClearDesc& clear_desc)
{
  decltype(auto) vk_framebuffer = framebuffer->As<VKFramebuffer>();
  decltype(auto) vk_render_pass = render_pass->As<VKRenderPass>();
  VkRenderPassBeginInfo render_pass_info = {};
  render_pass_info.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_info.renderPass = vk_render_pass.GetRenderPass();
  render_pass_info.framebuffer = vk_framebuffer.GetFramebuffer();
  render_pass_info.renderArea.extent = vk_framebuffer.GetExtent();
  std::vector<VkClearValue> clear_values;
  for (size_t i = 0; i < clear_desc.colors.size(); ++i)
  {
    auto& clear_value = clear_values.emplace_back();
    clear_value.color.float32[0] = clear_desc.colors[i].r;
    clear_value.color.float32[1] = clear_desc.colors[i].g;
    clear_value.color.float32[2] = clear_desc.colors[i].b;
    clear_value.color.float32[3] = clear_desc.colors[i].a;
  }
  clear_values.resize(vk_render_pass.GetDesc().colors.size());
  if (vk_render_pass.GetDesc().depth_stencil.format != ezRHIResourceFormat::UNKNOWN)
  {
    VkClearValue clear_value = {};
    clear_value.depthStencil.depth = clear_desc.depth;
    clear_value.depthStencil.stencil = clear_desc.stencil;
    clear_values.emplace_back(clear_value);
  }
  render_pass_info.clearValueCount = (ezUInt32)clear_values.size();
  render_pass_info.pClearValues = clear_values.data();
  vkCmdBeginRenderPass(m_command_list, &render_pass_info, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
}

void VKCommandList::EndRenderPass()
{
  vkCmdEndRenderPass(m_command_list);
}

void VKCommandList::BeginEvent(const std::string& name)
{
  VkDebugUtilsLabelEXT label = {};
  label.pLabelName = name.c_str();
  vkCmdBeginDebugUtilsLabelEXT(m_command_list, &label);
}

void VKCommandList::EndEvent()
{
  vkCmdEndDebugUtilsLabelEXT(m_command_list);
}

void VKCommandList::Draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance)
{
  vkCmdDraw(m_command_list, vertex_count, instance_count, first_vertex, first_instance);
}

void VKCommandList::DrawIndexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance)
{
  vkCmdDrawIndexed(m_command_list, index_count, instance_count, first_index, vertex_offset, first_instance);
}

void VKCommandList::DrawIndirect(const std::shared_ptr<Resource>& argument_buffer, uint64_t argument_buffer_offset)
{
  DrawIndirectCount(argument_buffer, argument_buffer_offset, {}, 0, 1, sizeof(DrawIndirectCommand));
}

void VKCommandList::DrawIndexedIndirect(const std::shared_ptr<Resource>& argument_buffer, uint64_t argument_buffer_offset)
{
  DrawIndexedIndirectCount(argument_buffer, argument_buffer_offset, {}, 0, 1, sizeof(DrawIndexedIndirectCommand));
}

void VKCommandList::DrawIndirectCount(
  const std::shared_ptr<Resource>& argument_buffer,
  uint64_t argument_buffer_offset,
  const std::shared_ptr<Resource>& count_buffer,
  uint64_t count_buffer_offset,
  uint32_t max_draw_count,
  uint32_t stride)
{
  decltype(auto) vk_argument_buffer = argument_buffer->As<VKResource>();
  if (count_buffer)
  {
    decltype(auto) vk_count_buffer = count_buffer->As<VKResource>();
    vkCmdDrawIndirectCount(
      m_command_list,
      vk_argument_buffer.buffer.res,
      argument_buffer_offset,
      vk_count_buffer.buffer.res,
      count_buffer_offset,
      max_draw_count,
      stride);
  }
  else
  {
    assert(count_buffer_offset == 0);
    vkCmdDrawIndirect(
      m_command_list,
      vk_argument_buffer.buffer.res,
      argument_buffer_offset,
      max_draw_count,
      stride);
  }
}

void VKCommandList::DrawIndexedIndirectCount(
  const std::shared_ptr<Resource>& argument_buffer,
  uint64_t argument_buffer_offset,
  const std::shared_ptr<Resource>& count_buffer,
  uint64_t count_buffer_offset,
  uint32_t max_draw_count,
  uint32_t stride)
{
  decltype(auto) vk_argument_buffer = argument_buffer->As<VKResource>();
  if (count_buffer)
  {
    decltype(auto) vk_count_buffer = count_buffer->As<VKResource>();
    vkCmdDrawIndexedIndirectCount(
      m_command_list,
      vk_argument_buffer.buffer.res,
      argument_buffer_offset,
      vk_count_buffer.buffer.res,
      count_buffer_offset,
      max_draw_count,
      stride);
  }
  else
  {
    assert(count_buffer_offset == 0);
    vkCmdDrawIndexedIndirect(
      m_command_list,
      vk_argument_buffer.buffer.res,
      argument_buffer_offset,
      max_draw_count,
      stride);
  }
}

void VKCommandList::Dispatch(uint32_t thread_group_count_x, uint32_t thread_group_count_y, uint32_t thread_group_count_z)
{
  vkCmdDispatch(m_command_list, thread_group_count_x, thread_group_count_y, thread_group_count_z);
}

void VKCommandList::DispatchIndirect(const std::shared_ptr<Resource>& argument_buffer, uint64_t argument_buffer_offset)
{
  decltype(auto) vk_argument_buffer = argument_buffer->As<VKResource>();
  vkCmdDispatchIndirect(
    m_command_list,
    vk_argument_buffer.buffer.res,
    argument_buffer_offset);
}

void VKCommandList::DispatchMesh(uint32_t thread_group_count_x)
{
  vkCmdDrawMeshTasksNV(m_command_list, thread_group_count_x, 0);
}

static VkStridedDeviceAddressRegionKHR GetStridedDeviceAddressRegion(VKDevice& device, const RayTracingShaderTable& table)
{
  if (!table.resource)
  {
    return {};
  }
  decltype(auto) vk_resource = table.resource->As<VKResource>();
  VkStridedDeviceAddressRegionKHR vk_table = {};
  VkBufferDeviceAddressInfo bufferAddressInfo = {};
  bufferAddressInfo.buffer = vk_resource.buffer.res;
  
  vk_table.deviceAddress = vkGetBufferDeviceAddress(device.GetDevice(), &bufferAddressInfo) + table.offset;
  vk_table.size = table.size;
  vk_table.stride = table.stride;
  return vk_table;
}

void VKCommandList::DispatchRays(const RayTracingShaderTables& shader_tables, uint32_t width, uint32_t height, uint32_t depth)
{
  VkStridedDeviceAddressRegionKHR raygen = GetStridedDeviceAddressRegion(m_device, shader_tables.raygen);
  VkStridedDeviceAddressRegionKHR miss = GetStridedDeviceAddressRegion(m_device, shader_tables.miss);
  VkStridedDeviceAddressRegionKHR hit = GetStridedDeviceAddressRegion(m_device, shader_tables.hit);
  VkStridedDeviceAddressRegionKHR callable = GetStridedDeviceAddressRegion(m_device, shader_tables.callable);

  vkCmdTraceRaysKHR(
    m_command_list,
    &raygen,
    &miss,
    &hit,
    &callable,
    width,
    height,
    depth);
}

void VKCommandList::ResourceBarrier(const std::vector<ResourceBarrierDesc>& barriers)
{
  std::vector<VkImageMemoryBarrier> image_memory_barriers;
  for (const auto& barrier : barriers)
  {
    if (!barrier.resource)
    {
      assert(false);
      continue;
    }

    decltype(auto) vk_resource = barrier.resource->As<VKResource>();
    VKResource::Image& image = vk_resource.image;
    if (!image.res)
      continue;

    VkImageLayout vk_state_before = ConvertState(barrier.state_before);
    VkImageLayout vk_state_after = ConvertState(barrier.state_after);
    if (vk_state_before == vk_state_after)
      continue;

    VkImageMemoryBarrier& image_memory_barrier = image_memory_barriers.emplace_back();
    image_memory_barrier = {};
    image_memory_barrier.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    image_memory_barrier.oldLayout = vk_state_before;
    image_memory_barrier.newLayout = vk_state_after;
    image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.image = image.res;
    image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

    VkImageSubresourceRange& range = image_memory_barrier.subresourceRange;
    range.aspectMask = m_device.GetAspectFlags(image.format);
    range.baseMipLevel = barrier.base_mip_level;
    range.levelCount = barrier.level_count;
    range.baseArrayLayer = barrier.base_array_layer;
    range.layerCount = barrier.layer_count;

    // Source layouts (old)
    // Source access mask controls actions that have to be finished on the old layout
    // before it will be transitioned to the new layout
    switch (image_memory_barrier.oldLayout)
    {
      case VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED:
        // Image layout is undefined (or does not matter)
        // Only valid as initial layout
        // No flags required, listed only for completeness
        image_memory_barrier.srcAccessMask = {};
        break;
      case VkImageLayout::VK_IMAGE_LAYOUT_PREINITIALIZED:
        // Image is preinitialized
        // Only valid as initial layout for linear images, preserves memory contents
        // Make sure host writes have been finished
        image_memory_barrier.srcAccessMask = VkAccessFlagBits::VK_ACCESS_HOST_WRITE_BIT;
        break;
      case VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        // Image is a color attachment
        // Make sure any writes to the color buffer have been finished
        image_memory_barrier.srcAccessMask = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        break;
      case VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
        // Image is a depth/stencil attachment
        // Make sure any writes to the depth/stencil buffer have been finished
        image_memory_barrier.srcAccessMask = VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        break;
      case VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        // Image is a transfer source
        // Make sure any reads from the image have been finished
        image_memory_barrier.srcAccessMask = VkAccessFlagBits::VK_ACCESS_TRANSFER_READ_BIT;
        break;
      case VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        // Image is a transfer destination
        // Make sure any writes to the image have been finished
        image_memory_barrier.srcAccessMask = VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT;
        break;

      case VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        // Image is read by a shader
        // Make sure any shader reads from the image have been finished
        image_memory_barrier.srcAccessMask = VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT;
        break;
      case VkImageLayout::VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR:
        image_memory_barrier.srcAccessMask = VkAccessFlagBits::VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR;
      default:
        // Other source layouts aren't handled (yet)
        break;
    }

    // Target layouts (new)
    // Destination access mask controls the dependency for the new image layout
    switch (image_memory_barrier.newLayout)
    {
      case VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        // Image will be used as a transfer destination
        // Make sure any writes to the image have been finished
        image_memory_barrier.dstAccessMask = VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT;
        break;

      case VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        // Image will be used as a transfer source
        // Make sure any reads from the image have been finished
        image_memory_barrier.dstAccessMask = VkAccessFlagBits::VK_ACCESS_TRANSFER_READ_BIT;
        break;

      case VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        // Image will be used as a color attachment
        // Make sure any writes to the color buffer have been finished
        image_memory_barrier.dstAccessMask = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        break;

      case VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
        // Image layout will be used as a depth/stencil attachment
        // Make sure any writes to depth/stencil buffer have been finished
        image_memory_barrier.dstAccessMask = image_memory_barrier.dstAccessMask | VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        break;

      case VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        // Image will be read in a shader (sampler, input attachment)
        // Make sure any writes to the image have been finished
        if (!image_memory_barrier.srcAccessMask)
        {
          image_memory_barrier.srcAccessMask = VkAccessFlagBits::VK_ACCESS_HOST_WRITE_BIT | VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT;
        }
        image_memory_barrier.dstAccessMask = VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT;
        break;
      case VkImageLayout::VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR:
        image_memory_barrier.dstAccessMask = VkAccessFlagBits::VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR;
        break;
      default:
        // Other source layouts aren't handled (yet)
        break;
    }
  }

  if (!image_memory_barriers.empty())
  {
    vkCmdPipelineBarrier(
      m_command_list,
      VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
      VkDependencyFlagBits::VK_DEPENDENCY_BY_REGION_BIT,
      0, nullptr,
      0, nullptr,
      (ezUInt32)image_memory_barriers.size(), image_memory_barriers.data());
  }
}

void VKCommandList::UAVResourceBarrier(const std::shared_ptr<Resource>& /*resource*/)
{
  VkMemoryBarrier memory_barrier = {};
  memory_barrier.srcAccessMask = VkAccessFlagBits::VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR | VkAccessFlagBits::VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR | VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT;
  memory_barrier.dstAccessMask = memory_barrier.srcAccessMask;
  vkCmdPipelineBarrier(m_command_list, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VkDependencyFlagBits::VK_DEPENDENCY_BY_REGION_BIT, 1, &memory_barrier, 0, 0, 0, 0);
}

void VKCommandList::SetViewport(float x, float y, float width, float height)
{
  VkViewport viewport = {};
  viewport.x = 0;
  viewport.y = height - y;
  viewport.width = width;
  viewport.height = -height;
  viewport.minDepth = 0;
  viewport.maxDepth = 1.0;
  vkCmdSetViewport(m_command_list, 0, 1, &viewport);
}

void VKCommandList::SetScissorRect(int32_t left, int32_t top, uint32_t right, uint32_t bottom)
{
  VkRect2D rect = {};
  rect.offset.x = left;
  rect.offset.y = top;
  rect.extent.width = right;
  rect.extent.height = bottom;
  vkCmdSetScissor(m_command_list, 0, 1, &rect);
}

void VKCommandList::IASetIndexBuffer(const std::shared_ptr<Resource>& resource, ezRHIResourceFormat::Enum format)
{
  decltype(auto) vk_resource = resource->As<VKResource>();
  VkIndexType index_type = VKUtils::GetVkIndexType(format);
  vkCmdBindIndexBuffer(m_command_list, vk_resource.buffer.res, 0, index_type);
}

void VKCommandList::IASetVertexBuffer(uint32_t slot, const std::shared_ptr<Resource>& resource)
{
  decltype(auto) vk_resource = resource->As<VKResource>();
  VkBuffer vertex_buffers[] = {vk_resource.buffer.res};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(m_command_list, slot, 1, vertex_buffers, offsets);
}

void VKCommandList::RSSetShadingRate(ShadingRate shading_rate, const std::array<ShadingRateCombiner, 2>& combiners)
{
  VkExtent2D fragment_size = {1, 1};
  switch (shading_rate)
  {
    case ShadingRate::k1x1:
      fragment_size.width = 1;
      fragment_size.height = 1;
      break;
    case ShadingRate::k1x2:
      fragment_size.width = 1;
      fragment_size.height = 2;
      break;
    case ShadingRate::k2x1:
      fragment_size.width = 2;
      fragment_size.height = 1;
      break;
    case ShadingRate::k2x2:
      fragment_size.width = 2;
      fragment_size.height = 2;
      break;
    case ShadingRate::k2x4:
      fragment_size.width = 2;
      fragment_size.height = 4;
      break;
    case ShadingRate::k4x2:
      fragment_size.width = 4;
      fragment_size.height = 2;
      break;
    case ShadingRate::k4x4:
      fragment_size.width = 4;
      fragment_size.height = 4;
      break;
    default:
      assert(false);
      break;
  }

  std::array<VkFragmentShadingRateCombinerOpKHR, 2> vk_combiners;
  for (size_t i = 0; i < vk_combiners.size(); ++i)
  {
    switch (combiners[i])
    {
      case ShadingRateCombiner::kPassthrough:
        vk_combiners[i] = VkFragmentShadingRateCombinerOpKHR::VK_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_KHR;
        break;
      case ShadingRateCombiner::kOverride:
        vk_combiners[i] = VkFragmentShadingRateCombinerOpKHR::VK_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE_KHR;
        break;
      case ShadingRateCombiner::kMin:
        vk_combiners[i] = VkFragmentShadingRateCombinerOpKHR::VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN_KHR;
        break;
      case ShadingRateCombiner::kMax:
        vk_combiners[i] = VkFragmentShadingRateCombinerOpKHR::VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX_KHR;
        break;
      case ShadingRateCombiner::kSum:
        vk_combiners[i] = VkFragmentShadingRateCombinerOpKHR::VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL_KHR;
        break;
      default:
        assert(false);
        break;
    }
  }

  vkCmdSetFragmentShadingRateKHR(m_command_list ,& fragment_size, vk_combiners.data());
}

void VKCommandList::BuildBottomLevelAS(
  const std::shared_ptr<Resource>& src,
  const std::shared_ptr<Resource>& dst,
  const std::shared_ptr<Resource>& scratch,
  uint64_t scratch_offset,
  const std::vector<RaytracingGeometryDesc>& descs,
  BuildAccelerationStructureFlags flags)
{
  std::vector<VkAccelerationStructureGeometryKHR> geometry_descs;
  for (const auto& desc : descs)
  {
    geometry_descs.emplace_back(m_device.FillRaytracingGeometryTriangles(desc.vertex, desc.index, desc.flags));
  }

  decltype(auto) vk_dst = dst->As<VKResource>();
  decltype(auto) vk_scratch = scratch->As<VKResource>();

  VkAccelerationStructureKHR vk_src_as = {};
  if (src)
  {
    decltype(auto) vk_src = src->As<VKResource>();
    vk_src_as = vk_src.acceleration_structure_handle;
  }

  std::vector<VkAccelerationStructureBuildRangeInfoKHR> ranges;
  for (const auto& desc : descs)
  {
    VkAccelerationStructureBuildRangeInfoKHR& offset = ranges.emplace_back();
    if (desc.index.res)
      offset.primitiveCount = desc.index.count / 3;
    else
      offset.primitiveCount = desc.vertex.count / 3;
  }
  std::vector<const VkAccelerationStructureBuildRangeInfoKHR*> range_infos(ranges.size());
  for (size_t i = 0; i < ranges.size(); ++i)
    range_infos[i] = &ranges[i];

  VkAccelerationStructureBuildGeometryInfoKHR infos = {};
  infos.type = VkAccelerationStructureTypeKHR::VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
  infos.flags = Convert(flags);
  infos.dstAccelerationStructure = vk_dst.acceleration_structure_handle;
  infos.srcAccelerationStructure = vk_src_as;
  if (vk_src_as)
    infos.mode = VkBuildAccelerationStructureModeKHR::VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR;
  else
    infos.mode = VkBuildAccelerationStructureModeKHR::VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;

  VkBufferDeviceAddressInfo scratchBufferAddressInfo = {};
  scratchBufferAddressInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
  scratchBufferAddressInfo.buffer = vk_scratch.buffer.res;
  infos.scratchData.deviceAddress = vkGetBufferDeviceAddress(m_device.GetDevice(), &scratchBufferAddressInfo) + scratch_offset;
  infos.pGeometries = geometry_descs.data();
  infos.geometryCount = (ezUInt32)geometry_descs.size();

  vkCmdBuildAccelerationStructuresKHR(m_command_list, 1, &infos, range_infos.data());
}

void VKCommandList::BuildTopLevelAS(
  const std::shared_ptr<Resource>& src,
  const std::shared_ptr<Resource>& dst,
  const std::shared_ptr<Resource>& scratch,
  uint64_t scratch_offset,
  const std::shared_ptr<Resource>& instance_data,
  uint64_t instance_offset,
  uint32_t instance_count,
  BuildAccelerationStructureFlags flags)
{
  decltype(auto) vk_instance_data = instance_data->As<VKResource>();
  VkBufferDeviceAddressInfo instanceDataBufferAddressInfo = {};
  instanceDataBufferAddressInfo.buffer = vk_instance_data.buffer.res;
  VkDeviceAddress instance_address = vkGetBufferDeviceAddress(m_device.GetDevice(), &instanceDataBufferAddressInfo) + instance_offset;

  VkAccelerationStructureGeometryKHR top_as_geometry = {};
  top_as_geometry.geometryType = VkGeometryTypeKHR::VK_GEOMETRY_TYPE_INSTANCES_KHR;
  top_as_geometry.geometry.instances = {};
  top_as_geometry.geometry.instances.arrayOfPointers = VK_FALSE;
  top_as_geometry.geometry.instances.data.deviceAddress = instance_address;

  decltype(auto) vk_dst = dst->As<VKResource>();
  decltype(auto) vk_scratch = scratch->As<VKResource>();

  VkAccelerationStructureKHR vk_src_as = {};
  if (src)
  {
    decltype(auto) vk_src = src->As<VKResource>();
    vk_src_as = vk_src.acceleration_structure_handle;
  }

  VkAccelerationStructureBuildRangeInfoKHR acceleration_structure_build_range_info = {};
  acceleration_structure_build_range_info.primitiveCount = instance_count;
  std::vector<VkAccelerationStructureBuildRangeInfoKHR*> offset_infos = {&acceleration_structure_build_range_info};

  VkAccelerationStructureBuildGeometryInfoKHR infos = {};
  infos.type = VkAccelerationStructureTypeKHR::VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
  infos.flags = Convert(flags);
  infos.dstAccelerationStructure = vk_dst.acceleration_structure_handle;
  infos.srcAccelerationStructure = vk_src_as;
  if (vk_src_as)
    infos.mode = VkBuildAccelerationStructureModeKHR::VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR;
  else
    infos.mode = VkBuildAccelerationStructureModeKHR::VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;

  VkBufferDeviceAddressInfo scratchBufferAddressInfo = {};
  scratchBufferAddressInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
  scratchBufferAddressInfo.buffer = vk_scratch.buffer.res;
  infos.scratchData.deviceAddress = vkGetBufferDeviceAddress(m_device.GetDevice(), &scratchBufferAddressInfo) + scratch_offset;
  infos.pGeometries = &top_as_geometry;
  infos.geometryCount = 1;

  vkCmdBuildAccelerationStructuresKHR(m_command_list, 1, &infos, offset_infos.data());
}

void VKCommandList::CopyAccelerationStructure(const std::shared_ptr<Resource>& src, const std::shared_ptr<Resource>& dst, CopyAccelerationStructureMode mode)
{
  decltype(auto) vk_src = src->As<VKResource>();
  decltype(auto) vk_dst = dst->As<VKResource>();
  VkCopyAccelerationStructureInfoKHR info = {};
  info.sType = VkStructureType::VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_INFO_KHR;
  switch (mode)
  {
    case CopyAccelerationStructureMode::kClone:
      info.mode = VkCopyAccelerationStructureModeKHR::VK_COPY_ACCELERATION_STRUCTURE_MODE_CLONE_KHR;
      break;
    case CopyAccelerationStructureMode::kCompact:
      info.mode = VkCopyAccelerationStructureModeKHR::VK_COPY_ACCELERATION_STRUCTURE_MODE_COMPACT_KHR;
      break;
    default:
      assert(false);
      info.mode = VkCopyAccelerationStructureModeKHR::VK_COPY_ACCELERATION_STRUCTURE_MODE_CLONE_KHR;
      break;
  }
  info.dst = vk_dst.acceleration_structure_handle;
  info.src = vk_src.acceleration_structure_handle;
  vkCmdCopyAccelerationStructureKHR(m_command_list, &info);
}

void VKCommandList::CopyBuffer(const std::shared_ptr<Resource>& src_buffer, const std::shared_ptr<Resource>& dst_buffer,
  const std::vector<BufferCopyRegion>& regions)
{
  decltype(auto) vk_src_buffer = src_buffer->As<VKResource>();
  decltype(auto) vk_dst_buffer = dst_buffer->As<VKResource>();
  std::vector<VkBufferCopy> vk_regions;
  for (const auto& region : regions)
  {
    vk_regions.emplace_back(VkBufferCopy{region.src_offset, region.dst_offset, region.num_bytes});
  }
  vkCmdCopyBuffer(m_command_list, vk_src_buffer.buffer.res, vk_dst_buffer.buffer.res, (ezUInt32)vk_regions.size(), vk_regions.data());
}

void VKCommandList::CopyBufferToTexture(const std::shared_ptr<Resource>& src_buffer, const std::shared_ptr<Resource>& dst_texture,
  const std::vector<BufferToTextureCopyRegion>& regions)
{
  decltype(auto) vk_src_buffer = src_buffer->As<VKResource>();
  decltype(auto) vk_dst_texture = dst_texture->As<VKResource>();
  std::vector<VkBufferImageCopy> vk_regions;
  auto format = dst_texture->GetFormat();
  for (const auto& region : regions)
  {
    auto& vk_region = vk_regions.emplace_back();
    vk_region.bufferOffset = region.buffer_offset;
    if (ezRHIResourceFormat::IsFormatBlockCompressed(format))
    {
      ezVec3U32 extent = ezRHIResourceFormat::GetBlockExtent(format);
      vk_region.bufferRowLength = region.buffer_row_pitch / ezRHIResourceFormat::GetBlockSize(format) * extent.x;
      vk_region.bufferImageHeight = ((region.texture_extent.height + extent.y - 1) / extent.y) * extent.x;
    }
    else
    {
      vk_region.bufferRowLength = region.buffer_row_pitch / ezRHIResourceFormat::GetFormatStride(format);
      vk_region.bufferImageHeight = region.texture_extent.height;
    }
    vk_region.imageSubresource.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
    vk_region.imageSubresource.mipLevel = region.texture_mip_level;
    vk_region.imageSubresource.baseArrayLayer = region.texture_array_layer;
    vk_region.imageSubresource.layerCount = 1;
    vk_region.imageOffset.x = region.texture_offset.x;
    vk_region.imageOffset.y = region.texture_offset.y;
    vk_region.imageOffset.z = region.texture_offset.z;
    vk_region.imageExtent.width = region.texture_extent.width;
    vk_region.imageExtent.height = region.texture_extent.height;
    vk_region.imageExtent.depth = region.texture_extent.depth;
  }
  vkCmdCopyBufferToImage(
    m_command_list,
    vk_src_buffer.buffer.res,
    vk_dst_texture.image.res,
    VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    (ezUInt32)vk_regions.size(),
    vk_regions.data());
}

void VKCommandList::CopyTexture(const std::shared_ptr<Resource>& src_texture, const std::shared_ptr<Resource>& dst_texture,
  const std::vector<TextureCopyRegion>& regions)
{
  decltype(auto) vk_src_texture = src_texture->As<VKResource>();
  decltype(auto) vk_dst_texture = dst_texture->As<VKResource>();
  std::vector<VkImageCopy> vk_regions;
  for (const auto& region : regions)
  {
    auto& vk_region = vk_regions.emplace_back();
    vk_region.srcSubresource.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
    vk_region.srcSubresource.mipLevel = region.src_mip_level;
    vk_region.srcSubresource.baseArrayLayer = region.src_array_layer;
    vk_region.srcSubresource.layerCount = 1;
    vk_region.srcOffset.x = region.src_offset.x;
    vk_region.srcOffset.y = region.src_offset.y;
    vk_region.srcOffset.z = region.src_offset.z;

    vk_region.dstSubresource.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
    vk_region.dstSubresource.mipLevel = region.dst_mip_level;
    vk_region.dstSubresource.baseArrayLayer = region.dst_array_layer;
    vk_region.dstSubresource.layerCount = 1;
    vk_region.dstOffset.x = region.dst_offset.x;
    vk_region.dstOffset.y = region.dst_offset.y;
    vk_region.dstOffset.z = region.dst_offset.z;

    vk_region.extent.width = region.extent.width;
    vk_region.extent.height = region.extent.height;
    vk_region.extent.depth = region.extent.depth;
  }
  vkCmdCopyImage(m_command_list, vk_src_texture.image.res, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, vk_dst_texture.image.res, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, (ezUInt32)vk_regions.size(), vk_regions.data());
}

void VKCommandList::WriteAccelerationStructuresProperties(
  const std::vector<std::shared_ptr<Resource>>& acceleration_structures,
  const std::shared_ptr<QueryHeap>& query_heap,
  uint32_t first_query)
{
  std::vector<VkAccelerationStructureKHR> vk_acceleration_structures;
  vk_acceleration_structures.reserve(acceleration_structures.size());
  for (const auto& acceleration_structure : acceleration_structures)
  {
    vk_acceleration_structures.emplace_back(acceleration_structure->As<VKResource>().acceleration_structure_handle);
  }
  decltype(auto) vk_query_heap = query_heap->As<VKQueryHeap>();
  auto query_type = vk_query_heap.GetQueryType();
  assert(query_type == VkQueryType::VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_KHR);
  vkCmdResetQueryPool(m_command_list, vk_query_heap.GetQueryPool(), first_query, (ezUInt32)acceleration_structures.size());
  vkCmdWriteAccelerationStructuresPropertiesKHR(
    m_command_list,
    (ezUInt32)vk_acceleration_structures.size(),
    vk_acceleration_structures.data(),
    query_type,
    vk_query_heap.GetQueryPool(),
    first_query);
}

void VKCommandList::ResolveQueryData(
  const std::shared_ptr<QueryHeap>& query_heap,
  uint32_t first_query,
  uint32_t query_count,
  const std::shared_ptr<Resource>& dst_buffer,
  uint64_t dst_offset)
{
  decltype(auto) vk_query_heap = query_heap->As<VKQueryHeap>();
  auto query_type = vk_query_heap.GetQueryType();
  assert(query_type == VkQueryType::VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_KHR);
  vkCmdCopyQueryPoolResults(
    m_command_list,
    vk_query_heap.GetQueryPool(),
    first_query,
    query_count,
    dst_buffer->As<VKResource>().buffer.res,
    dst_offset,
    sizeof(uint64_t),
    VkQueryResultFlagBits::VK_QUERY_RESULT_WAIT_BIT);
}

VkCommandBuffer& VKCommandList::GetCommandList()
{
  return m_command_list;
}
