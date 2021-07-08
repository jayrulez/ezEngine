#include <RHIVulkanPCH.h>

#include <RHIVulkan/CommandList/VKCommandList.h>
#include <RHIVulkan/Resource/VKResource.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/Framebuffer/VKFramebuffer.h>
#include <RHIVulkan/RenderPass/VKRenderPass.h>
#include <RHIVulkan/BindingSet/VKBindingSet.h>
#include <RHIVulkan/QueryHeap/VKQueryHeap.h>
#include <RHIVulkan/Pipeline/VKPipeline.h>

vk::PipelineBindPoint GetPipelineBindPoint(ezRHIPipelineType type)
{
  switch (type)
  {
    case ezRHIPipelineType::Graphics:
      return vk::PipelineBindPoint::eGraphics;
    case ezRHIPipelineType::Compute:
      return vk::PipelineBindPoint::eCompute;
    case ezRHIPipelineType::RayTracing:
      return vk::PipelineBindPoint::eRayTracingKHR;
  }
  EZ_ASSERT_NOT_IMPLEMENTED;
  return {};
}

static vk::StridedDeviceAddressRegionKHR GetStridedDeviceAddressRegion(ezRHIVKDevice& device, const ezRHIRayTracingShaderTable& table)
{
  if (!table.m_Resource)
  {
    return {};
  }
  decltype(auto) vkResource = table.m_Resource->As<ezRHIVKResource>();
  vk::StridedDeviceAddressRegionKHR vkTable = {};
  vkTable.deviceAddress = device.GetDevice().getBufferAddress({vkResource->m_Buffer.m_Buffer.get()}) + table.m_Offset;
  vkTable.size = table.m_Size;
  vkTable.stride = table.m_Stride;
  return vkTable;
}

static vk::IndexType GetVkIndexType(ezRHIResourceFormat::Enum format)
{
  vk::Format vkFormat = VKUtilities::ToVKFormat(format);
  switch (vkFormat)
  {
    case vk::Format::eR16Uint:
      return vk::IndexType::eUint16;
    case vk::Format::eR32Uint:
      return vk::IndexType::eUint32;
    default:
      EZ_ASSERT_NOT_IMPLEMENTED;
      return {};
  }
}

ezRHIVKCommandList::ezRHIVKCommandList(ezRHIVKDevice& device, ezRHICommandListType type)
  : m_Device{device}
{
  vk::CommandBufferAllocateInfo cmdBufAllocInfo = {};
  cmdBufAllocInfo.commandPool = device.GetCmdPool(type);
  cmdBufAllocInfo.commandBufferCount = 1;
  cmdBufAllocInfo.level = vk::CommandBufferLevel::ePrimary;
  std::vector<vk::UniqueCommandBuffer> cmdBufs = device.GetDevice().allocateCommandBuffersUnique(cmdBufAllocInfo);
  m_CommandList = std::move(cmdBufs.front());
  vk::CommandBufferBeginInfo beginInfo = {};
  m_CommandList->begin(beginInfo);
}

vk::CommandBuffer ezRHIVKCommandList::GetCommandList()
{
  return m_CommandList.get();
}

void ezRHIVKCommandList::ResetPlatform()
{
  Close();
  vk::CommandBufferBeginInfo beginInfo = {};
  m_CommandList->begin(beginInfo);
  m_Closed = false;
  m_State = nullptr;
  m_BindingSet = nullptr;
}

void ezRHIVKCommandList::ClosePlatform()
{
  if (!m_Closed)
  {
    m_CommandList->end();
    m_Closed = true;
  }
}

void ezRHIVKCommandList::BindPipelinePlatform(ezRHIPipeline* pState)
{
  if (pState == m_State)
    return;
  m_State = static_cast<ezRHIVKPipeline*>(pState);
  m_CommandList->bindPipeline(GetPipelineBindPoint(m_State->GetPipelineType()), m_State->GetPipeline());
}

void ezRHIVKCommandList::BindBindingSetPlatform(ezRHIBindingSet* pBindingSet)
{
  if (pBindingSet == m_BindingSet)
    return;
  m_BindingSet = pBindingSet;
  decltype(auto) vkBindingSet = pBindingSet->As<ezRHIVKBindingSet>();
  decltype(auto) descriptorSets = vkBindingSet->GetDescriptorSets();
  if (descriptorSets.IsEmpty())
    return;
  m_CommandList->bindDescriptorSets(GetPipelineBindPoint(m_State->GetPipelineType()), m_State->GetPipelineLayout(), 0, descriptorSets.GetCount(), descriptorSets.GetData(), 0, nullptr);
}

void ezRHIVKCommandList::BeginRenderPassPlatform(const ezRHIRenderPass* pRenderPass, const ezRHIFramebuffer* pFramebuffer, const ezRHIClearDescription& clearDesc)
{
  decltype(auto) vkFramebuffer = pFramebuffer->As<ezRHIVKFramebuffer>();
  decltype(auto) vkRenderPass = pRenderPass->As<ezRHIVKRenderPass>();
  vk::RenderPassBeginInfo renderPassInfo = {};
  renderPassInfo.renderPass = vkRenderPass->GetRenderPass();
  renderPassInfo.framebuffer = vkFramebuffer->GetFramebuffer();
  renderPassInfo.renderArea.extent = vkFramebuffer->GetExtent();
  ezDynamicArray<vk::ClearValue> clearValues;
  for (ezUInt32 i = 0; i < clearDesc.m_Colors.GetCount(); ++i)
  {
    auto& clear_value = clearValues.ExpandAndGetRef();
    clear_value.color.float32[0] = clearDesc.m_Colors[i].r;
    clear_value.color.float32[1] = clearDesc.m_Colors[i].g;
    clear_value.color.float32[2] = clearDesc.m_Colors[i].b;
    clear_value.color.float32[3] = clearDesc.m_Colors[i].a;
  }
  clearValues.SetCount(vkRenderPass->GetDescription().m_Colors.GetCount());
  if (vkRenderPass->GetDescription().m_DepthStencil.m_Format != ezRHIResourceFormat::Unknown)
  {
    vk::ClearValue clearValue = {};
    clearValue.depthStencil.depth = clearDesc.m_Depth;
    clearValue.depthStencil.stencil = clearDesc.m_Stencil;
    clearValues.PushBack(clearValue);
  }
  renderPassInfo.clearValueCount = clearValues.GetCount();
  renderPassInfo.pClearValues = clearValues.GetData();
  m_CommandList->beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
}

void ezRHIVKCommandList::EndRenderPassPlatform()
{
  m_CommandList->endRenderPass();
}

void ezRHIVKCommandList::BeginEventPlatform(const ezString& name)
{
  vk::DebugUtilsLabelEXT label = {};
  label.pLabelName = name.GetData();
  m_CommandList->beginDebugUtilsLabelEXT(&label);
}

void ezRHIVKCommandList::EndEventPlatform()
{
  m_CommandList->endDebugUtilsLabelEXT();
}

void ezRHIVKCommandList::DrawPlatform(ezUInt32 vertexCount, ezUInt32 instanceCount, ezUInt32 firstVertex, ezUInt32 firstInstance)
{
  m_CommandList->draw(vertexCount, instanceCount, firstVertex, firstInstance);
}

void ezRHIVKCommandList::DrawIndexedPlatform(ezUInt32 indexCount, ezUInt32 instanceCount, ezUInt32 firstIndex, ezInt32 vertexOffset, ezUInt32 firstInstance)
{
  m_CommandList->drawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void ezRHIVKCommandList::DrawIndirectPlatform(const ezRHIResource* pArgumentBuffer, ezUInt64 argumentBufferOffset)
{
  DrawIndirectCount(pArgumentBuffer, argumentBufferOffset, {}, 0, 1, sizeof(ezRHIDrawIndirectCommand));
}

void ezRHIVKCommandList::DrawIndexedIndirectPlatform(const ezRHIResource* pArgumentBuffer, ezUInt64 argumentBufferOffset)
{
  DrawIndexedIndirectCount(pArgumentBuffer, argumentBufferOffset, {}, 0, 1, sizeof(ezRHIDrawIndexedIndirectCommand));
}

void ezRHIVKCommandList::DrawIndirectCountPlatform(const ezRHIResource* pArgumentBuffer, ezUInt64 argumentBufferOffset, const ezRHIResource* pCountBuffer, ezUInt64 countBufferOffset, ezUInt32 maxDrawCount, ezUInt32 stride)
{
  decltype(auto) vkArgumentBuffer = pArgumentBuffer->As<ezRHIVKResource>();
  if (pCountBuffer)
  {
    decltype(auto) vkCountBuffer = pCountBuffer->As<ezRHIVKResource>();
    m_CommandList->drawIndirectCount(
      vkArgumentBuffer->m_Buffer.m_Buffer.get(),
      argumentBufferOffset,
      vkCountBuffer->m_Buffer.m_Buffer.get(),
      countBufferOffset,
      maxDrawCount,
      stride);
  }
  else
  {
    EZ_ASSERT_ALWAYS(countBufferOffset == 0, "");
    m_CommandList->drawIndirect(
      vkArgumentBuffer->m_Buffer.m_Buffer.get(),
      argumentBufferOffset,
      maxDrawCount,
      stride);
  }
}

void ezRHIVKCommandList::DrawIndexedIndirectCountPlatform(const ezRHIResource* pArgumentBuffer, ezUInt64 argumentBufferOffset, const ezRHIResource* pCountBuffer, ezUInt64 countBufferOffset, ezUInt32 maxDrawCount, ezUInt32 stride)
{
  decltype(auto) vkArgumentBuffer = pArgumentBuffer->As<ezRHIVKResource>();
  if (pCountBuffer)
  {
    decltype(auto) vkCountBuffer = pCountBuffer->As<ezRHIVKResource>();
    m_CommandList->drawIndexedIndirectCount(
      vkArgumentBuffer->m_Buffer.m_Buffer.get(),
      argumentBufferOffset,
      vkCountBuffer->m_Buffer.m_Buffer.get(),
      countBufferOffset,
      maxDrawCount,
      stride);
  }
  else
  {
    EZ_ASSERT_ALWAYS(countBufferOffset == 0, "");
    m_CommandList->drawIndexedIndirect(
      vkArgumentBuffer->m_Buffer.m_Buffer.get(),
      argumentBufferOffset,
      maxDrawCount,
      stride);
  }
}

void ezRHIVKCommandList::DispatchPlatform(ezUInt32 threadGroupCountX, ezUInt32 threadGroupCountY, ezUInt32 threadGroupCountZ)
{
  m_CommandList->dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}

void ezRHIVKCommandList::DispatchIndirectPlatform(const ezRHIResource* pArgumentBuffer, ezUInt64 argumentBufferOffset)
{
  decltype(auto) vkArgumentBuffer = pArgumentBuffer->As<ezRHIVKResource>();
  m_CommandList->dispatchIndirect(
    vkArgumentBuffer->m_Buffer.m_Buffer.get(),
    argumentBufferOffset);
}

void ezRHIVKCommandList::DispatchMeshPlatform(ezUInt32 threadGroupCountX)
{
  m_CommandList->drawMeshTasksNV(threadGroupCountX, 0);
}

void ezRHIVKCommandList::DispatchRaysPlatform(const ezRHIRayTracingShaderTables& shaderTables, ezUInt32 width, ezUInt32 height, ezUInt32 depth)
{
  m_CommandList->traceRaysKHR(
    GetStridedDeviceAddressRegion(m_Device, shaderTables.m_Raygen),
    GetStridedDeviceAddressRegion(m_Device, shaderTables.m_Miss),
    GetStridedDeviceAddressRegion(m_Device, shaderTables.m_Hit),
    GetStridedDeviceAddressRegion(m_Device, shaderTables.m_Callable),
    width,
    height,
    depth);
}

void ezRHIVKCommandList::ResourceBarrierPlatform(const ezDynamicArray<ezRHIResourceBarrierDescription>& barriers)
{
  ezDynamicArray<vk::ImageMemoryBarrier> imageMemoryBarriers;
  for (const auto& barrier : barriers)
  {
    if (!barrier.m_Resource)
    {
      EZ_ASSERT_ALWAYS(false, "");
      continue;
    }

    decltype(auto) vkResource = barrier.m_Resource->As<ezRHIVKResource>();
    ezRHIVKResource::Image& image = vkResource->m_Image;
    if (!image.m_Image)
      continue;

    vk::ImageLayout vkStateBefore = ConvertState(barrier.m_StateBefore);
    vk::ImageLayout vkStateAfter = ConvertState(barrier.m_StateAfter);
    if (vkStateBefore == vkStateAfter)
      continue;

    vk::ImageMemoryBarrier& imageMemoryBarrier = imageMemoryBarriers.ExpandAndGetRef();
    imageMemoryBarrier.oldLayout = vkStateBefore;
    imageMemoryBarrier.newLayout = vkStateAfter;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.image = image.m_Image;

    vk::ImageSubresourceRange& range = imageMemoryBarrier.subresourceRange;
    range.aspectMask = m_Device.GetAspectFlags(image.m_Format);
    range.baseMipLevel = barrier.m_BaseMipLevel;
    range.levelCount = barrier.m_LevelCount;
    range.baseArrayLayer = barrier.m_BaseArrayLayer;
    range.layerCount = barrier.m_LayerCount;

    // Source layouts (old)
    // Source access mask controls actions that have to be finished on the old layout
    // before it will be transitioned to the new layout
    switch (imageMemoryBarrier.oldLayout)
    {
      case vk::ImageLayout::eUndefined:
        // Image layout is undefined (or does not matter)
        // Only valid as initial layout
        // No flags required, listed only for completeness
        imageMemoryBarrier.srcAccessMask = {};
        break;
      case vk::ImageLayout::ePreinitialized:
        // Image is preinitialized
        // Only valid as initial layout for linear images, preserves memory contents
        // Make sure host writes have been finished
        imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eHostWrite;
        break;
      case vk::ImageLayout::eColorAttachmentOptimal:
        // Image is a color attachment
        // Make sure any writes to the color buffer have been finished
        imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
        break;
      case vk::ImageLayout::eDepthAttachmentOptimal:
        // Image is a depth/stencil attachment
        // Make sure any writes to the depth/stencil buffer have been finished
        imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        break;
      case vk::ImageLayout::eTransferSrcOptimal:
        // Image is a transfer source
        // Make sure any reads from the image have been finished
        imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
        break;
      case vk::ImageLayout::eTransferDstOptimal:
        // Image is a transfer destination
        // Make sure any writes to the image have been finished
        imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        break;

      case vk::ImageLayout::eShaderReadOnlyOptimal:
        // Image is read by a shader
        // Make sure any shader reads from the image have been finished
        imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eShaderRead;
        break;
      case vk::ImageLayout::eFragmentShadingRateAttachmentOptimalKHR:
        imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eFragmentShadingRateAttachmentReadKHR;
      default:
        // Other source layouts aren't handled (yet)
        break;
    }

    // Target layouts (new)
    // Destination access mask controls the dependency for the new image layout
    switch (imageMemoryBarrier.newLayout)
    {
      case vk::ImageLayout::eTransferDstOptimal:
        // Image will be used as a transfer destination
        // Make sure any writes to the image have been finished
        imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
        break;

      case vk::ImageLayout::eTransferSrcOptimal:
        // Image will be used as a transfer source
        // Make sure any reads from the image have been finished
        imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
        break;

      case vk::ImageLayout::eColorAttachmentOptimal:
        // Image will be used as a color attachment
        // Make sure any writes to the color buffer have been finished
        imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
        break;

      case vk::ImageLayout::eDepthAttachmentOptimal:
        // Image layout will be used as a depth/stencil attachment
        // Make sure any writes to depth/stencil buffer have been finished
        imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        break;

      case vk::ImageLayout::eShaderReadOnlyOptimal:
        // Image will be read in a shader (sampler, input attachment)
        // Make sure any writes to the image have been finished
        if (!imageMemoryBarrier.srcAccessMask)
        {
          imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eHostWrite | vk::AccessFlagBits::eTransferWrite;
        }
        imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
        break;
      case vk::ImageLayout::eFragmentShadingRateAttachmentOptimalKHR:
        imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eFragmentShadingRateAttachmentReadKHR;
        break;
      default:
        // Other source layouts aren't handled (yet)
        break;
    }
  }

  if (!imageMemoryBarriers.IsEmpty())
  {
    m_CommandList->pipelineBarrier(
      vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eAllCommands,
      vk::DependencyFlagBits::eByRegion,
      0, nullptr,
      0, nullptr,
      imageMemoryBarriers.GetCount(), imageMemoryBarriers.GetData());
  }
}

void ezRHIVKCommandList::UAVResourceBarrierPlatform(const ezRHIResource* pResource)
{
  vk::MemoryBarrier memoryBarrier = {};
  memoryBarrier.srcAccessMask = vk::AccessFlagBits::eAccelerationStructureWriteKHR | vk::AccessFlagBits::eAccelerationStructureReadKHR | vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eShaderRead;
  memoryBarrier.dstAccessMask = memoryBarrier.srcAccessMask;
  m_CommandList->pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eAllCommands, vk::DependencyFlagBits::eByRegion, 1, &memoryBarrier, 0, 0, 0, 0);
}

void ezRHIVKCommandList::SetViewportPlatform(float x, float y, float width, float height)
{
  vk::Viewport viewport = {};
  viewport.x = 0;
  viewport.y = height - y;
  viewport.width = width;
  viewport.height = -height;
  viewport.minDepth = 0;
  viewport.maxDepth = 1.0;
  m_CommandList->setViewport(0, 1, &viewport);
}

void ezRHIVKCommandList::SetScissorRectPlatform(ezInt32 left, ezInt32 top, ezUInt32 right, ezUInt32 bottom)
{
  vk::Rect2D rect = {};
  rect.offset.x = left;
  rect.offset.y = top;
  rect.extent.width = right;
  rect.extent.height = bottom;
  m_CommandList->setScissor(0, 1, &rect);
}

void ezRHIVKCommandList::IASetIndexBufferPlatform(const ezRHIResource* pResource, ezRHIResourceFormat::Enum format)
{
  decltype(auto) vkResource = pResource->As<ezRHIVKResource>();
  vk::IndexType indexType = GetVkIndexType(format);
  m_CommandList->bindIndexBuffer(vkResource->m_Buffer.m_Buffer.get(), 0, indexType);
}

void ezRHIVKCommandList::IASetVertexBufferPlatform(ezUInt32 slot, const ezRHIResource* pResource)
{
  decltype(auto) vkResource = pResource->As<ezRHIVKResource>();
  vk::Buffer vertexBuffers[] = {vkResource->m_Buffer.m_Buffer.get()};
  vk::DeviceSize offsets[] = {0};
  m_CommandList->bindVertexBuffers(slot, 1, vertexBuffers, offsets);
}

void ezRHIVKCommandList::RSSetShadingRatePlatform(ezRHIShadingRate shadingRate, const ezStaticArray<ezRHIShadingRateCombiner, 2>& combiners)
{
  vk::Extent2D fragmentSize = {1, 1};
  switch (shadingRate)
  {
    case ezRHIShadingRate::ShadingRate1x1:
      fragmentSize.width = 1;
      fragmentSize.height = 1;
      break;
    case ezRHIShadingRate::ShadingRate1x2:
      fragmentSize.width = 1;
      fragmentSize.height = 2;
      break;
    case ezRHIShadingRate::ShadingRate2x1:
      fragmentSize.width = 2;
      fragmentSize.height = 1;
      break;
    case ezRHIShadingRate::ShadingRate2x2:
      fragmentSize.width = 2;
      fragmentSize.height = 2;
      break;
    case ezRHIShadingRate::ShadingRate2x4:
      fragmentSize.width = 2;
      fragmentSize.height = 4;
      break;
    case ezRHIShadingRate::ShadingRate4x2:
      fragmentSize.width = 4;
      fragmentSize.height = 2;
      break;
    case ezRHIShadingRate::ShadingRate4x4:
      fragmentSize.width = 4;
      fragmentSize.height = 4;
      break;
    default:
      EZ_ASSERT_NOT_IMPLEMENTED;
      break;
  }

  ezStaticArray<vk::FragmentShadingRateCombinerOpKHR, 2> vkCombiners;
  for (ezUInt32 i = 0; i < vkCombiners.GetCount(); ++i)
  {
    switch (combiners[i])
    {
      case ezRHIShadingRateCombiner::Passthrough:
        vkCombiners[i] = vk::FragmentShadingRateCombinerOpKHR::eKeep;
        break;
      case ezRHIShadingRateCombiner::Override:
        vkCombiners[i] = vk::FragmentShadingRateCombinerOpKHR::eReplace;
        break;
      case ezRHIShadingRateCombiner::Min:
        vkCombiners[i] = vk::FragmentShadingRateCombinerOpKHR::eMin;
        break;
      case ezRHIShadingRateCombiner::Max:
        vkCombiners[i] = vk::FragmentShadingRateCombinerOpKHR::eMax;
        break;
      case ezRHIShadingRateCombiner::Sum:
        vkCombiners[i] = vk::FragmentShadingRateCombinerOpKHR::eMul;
        break;
      default:
        EZ_ASSERT_NOT_IMPLEMENTED;
        break;
    }
  }

  m_CommandList->setFragmentShadingRateKHR(&fragmentSize, vkCombiners.GetData());
}

void ezRHIVKCommandList::BuildBottomLevelASPlatform(const ezRHIResource* pSrc, const ezRHIResource* pDst, const ezRHIResource* pScratch, ezUInt64 scratchOffset, const ezDynamicArray<ezRHIRaytracingGeometryDescription>& descs, ezBitflags<ezRHIBuildAccelerationStructureFlags> flags)
{
  ezDynamicArray<vk::AccelerationStructureGeometryKHR> geometryDescs;
  for (const auto& desc : descs)
  {
    geometryDescs.PushBack(m_Device.FillRaytracingGeometryTriangles(desc.m_Vertex, desc.m_Index, desc.m_Flags));
  }

  decltype(auto) vkDst = pDst->As<ezRHIVKResource>();
  decltype(auto) vkScratch = pScratch->As<ezRHIVKResource>();

  vk::AccelerationStructureKHR vkSrcAS = {};
  if (pSrc)
  {
    decltype(auto) vkSrc = pSrc->As<ezRHIVKResource>();
    vkSrcAS = vkSrc->m_AccelerationStructureHandle.get();
  }

  ezDynamicArray<vk::AccelerationStructureBuildRangeInfoKHR> ranges;
  for (const auto& desc : descs)
  {
    vk::AccelerationStructureBuildRangeInfoKHR& offset = ranges.ExpandAndGetRef();
    if (desc.m_Index.m_Resource)
      offset.primitiveCount = desc.m_Index.m_Count / 3;
    else
      offset.primitiveCount = desc.m_Vertex.m_Count / 3;
  }
  ezDynamicArray<const vk::AccelerationStructureBuildRangeInfoKHR*> rangeInfos;
  rangeInfos.SetCount(ranges.GetCount());

  for (ezUInt32 i = 0; i < ranges.GetCount(); ++i)
    rangeInfos[i] = &ranges[i];

  vk::AccelerationStructureBuildGeometryInfoKHR infos = {};
  infos.type = vk::AccelerationStructureTypeKHR::eBottomLevel;
  infos.flags = Convert(flags);
  infos.dstAccelerationStructure = vkDst->m_AccelerationStructureHandle.get();
  infos.srcAccelerationStructure = vkSrcAS;
  if (vkSrcAS)
    infos.mode = vk::BuildAccelerationStructureModeKHR::eUpdate;
  else
    infos.mode = vk::BuildAccelerationStructureModeKHR::eBuild;
  infos.scratchData = m_Device.GetDevice().getBufferAddress({vkScratch->m_Buffer.m_Buffer.get()}) + scratchOffset;
  infos.pGeometries = geometryDescs.GetData();
  infos.geometryCount = geometryDescs.GetCount();

  m_CommandList->buildAccelerationStructuresKHR(1, &infos, rangeInfos.GetData());
}

void ezRHIVKCommandList::BuildTopLevelASPlatform(const ezRHIResource* pSrc, const ezRHIResource* pDst, const ezRHIResource* pScratch, ezUInt64 scratchOffset, const ezRHIResource* pInstanceData, ezUInt64 instanceOffset, ezUInt32 instanceCount, ezBitflags<ezRHIBuildAccelerationStructureFlags> flags)
{
  decltype(auto) vkInstanceData = pInstanceData->As<ezRHIVKResource>();
  vk::DeviceAddress instanceAddress = m_Device.GetDevice().getBufferAddress(vkInstanceData->m_Buffer.m_Buffer.get()) + instanceOffset;

  vk::AccelerationStructureGeometryKHR topASGeometry = {};
  topASGeometry.geometryType = vk::GeometryTypeKHR::eInstances;
  topASGeometry.geometry.setInstances({});
  topASGeometry.geometry.instances.arrayOfPointers = VK_FALSE;
  topASGeometry.geometry.instances.data = instanceAddress;

  decltype(auto) vkDst = pDst->As<ezRHIVKResource>();
  decltype(auto) vkScratch = pScratch->As<ezRHIVKResource>();

  vk::AccelerationStructureKHR vkSrcAS = {};
  if (pSrc)
  {
    decltype(auto) vkSrc = pSrc->As<ezRHIVKResource>();
    vkSrcAS = vkSrc->m_AccelerationStructureHandle.get();
  }

  vk::AccelerationStructureBuildRangeInfoKHR accelerationStructureBuildRangeInfo = {};
  accelerationStructureBuildRangeInfo.primitiveCount = instanceCount;
  ezDynamicArray<vk::AccelerationStructureBuildRangeInfoKHR*> offsetInfos;
  offsetInfos.PushBack(&accelerationStructureBuildRangeInfo);

  vk::AccelerationStructureBuildGeometryInfoKHR infos = {};
  infos.type = vk::AccelerationStructureTypeKHR::eTopLevel;
  infos.flags = Convert(flags);
  infos.dstAccelerationStructure = vkDst->m_AccelerationStructureHandle.get();
  infos.srcAccelerationStructure = vkSrcAS;
  if (vkSrcAS)
    infos.mode = vk::BuildAccelerationStructureModeKHR::eUpdate;
  else
    infos.mode = vk::BuildAccelerationStructureModeKHR::eBuild;
  infos.scratchData = m_Device.GetDevice().getBufferAddress({vkScratch->m_Buffer.m_Buffer.get()}) + scratchOffset;
  infos.pGeometries = &topASGeometry;
  infos.geometryCount = 1;

  m_CommandList->buildAccelerationStructuresKHR(1, &infos, offsetInfos.GetData());
}

void ezRHIVKCommandList::CopyAccelerationStructurePlatform(const ezRHIResource* pSrc, const ezRHIResource* pDst, ezRHICopyAccelerationStructureMode mode)
{
  decltype(auto) vkSrc = pSrc->As<ezRHIVKResource>();
  decltype(auto) vkDst = pDst->As<ezRHIVKResource>();
  vk::CopyAccelerationStructureInfoKHR info = {};
  switch (mode)
  {
    case ezRHICopyAccelerationStructureMode::Clone:
      info.mode = vk::CopyAccelerationStructureModeKHR::eClone;
      break;
    case ezRHICopyAccelerationStructureMode::Compact:
      info.mode = vk::CopyAccelerationStructureModeKHR::eCompact;
      break;
    default:
      assert(false);
      info.mode = vk::CopyAccelerationStructureModeKHR::eClone;
      break;
  }
  info.dst = vkDst->m_AccelerationStructureHandle.get();
  info.src = vkSrc->m_AccelerationStructureHandle.get();
  m_CommandList->copyAccelerationStructureKHR(info);
}

void ezRHIVKCommandList::CopyBufferPlatform(const ezRHIResource* pSrcBuffer, const ezRHIResource* pDstBuffer, const ezDynamicArray<ezRHIBufferCopyRegion>& regions)
{
  decltype(auto) vkSrcBuffer = pSrcBuffer->As<ezRHIVKResource>();
  decltype(auto) vkDstBuffer = pDstBuffer->As<ezRHIVKResource>();
  std::vector<vk::BufferCopy> vkRegions;
  for (const auto& region : regions)
  {
    vkRegions.emplace_back(vk::BufferCopy(region.m_SrcOffset, region.m_DstOffset, region.m_NumBytes));
  }
  m_CommandList->copyBuffer(vkSrcBuffer->m_Buffer.m_Buffer.get(), vkDstBuffer->m_Buffer.m_Buffer.get(), vkRegions);
}

void ezRHIVKCommandList::CopyBufferToTexturePlatform(const ezRHIResource* pSrcBuffer, const ezRHIResource* DstTexture, const ezDynamicArray<ezRHIBufferToTextureCopyRegion>& regions)
{
  decltype(auto) vkSrcBuffer = pSrcBuffer->As<ezRHIVKResource>();
  decltype(auto) vkDstTexture = DstTexture->As<ezRHIVKResource>();
  std::vector<vk::BufferImageCopy> vkRegions;
  auto format = DstTexture->GetFormat();
  for (const auto& region : regions)
  {
    auto& vkRegion = vkRegions.emplace_back();
    vkRegion.bufferOffset = region.m_BufferOffset;
    if (ezRHIResourceFormat::IsFormatBlockCompressed(format))
    {
      auto extent = ezRHIResourceFormat::GetBlockExtent(format);
      vkRegion.bufferRowLength = region.m_BufferRowPitch / ezRHIResourceFormat::GetBlockSize(format) * extent.x;
      vkRegion.bufferImageHeight = ((region.m_TextureExtent.m_Height + extent.y - 1) / extent.y) * extent.x;
    }
    else
    {
      vkRegion.bufferRowLength = region.m_BufferRowPitch /  ezRHIResourceFormat::GetFormatStride(format);
      vkRegion.bufferImageHeight = region.m_TextureExtent.m_Height;
    }
    vkRegion.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    vkRegion.imageSubresource.mipLevel = region.m_TextureMipLevel;
    vkRegion.imageSubresource.baseArrayLayer = region.m_TextureArrayLayer;
    vkRegion.imageSubresource.layerCount = 1;
    vkRegion.imageOffset.x = region.m_TextureOffset.x;
    vkRegion.imageOffset.y = region.m_TextureOffset.y;
    vkRegion.imageOffset.z = region.m_TextureOffset.z;
    vkRegion.imageExtent.width = region.m_TextureExtent.m_Width;
    vkRegion.imageExtent.height = region.m_TextureExtent.m_Height;
    vkRegion.imageExtent.depth = region.m_TextureExtent.m_Depth;
  }
  m_CommandList->copyBufferToImage(
    vkSrcBuffer->m_Buffer.m_Buffer.get(),
    vkDstTexture->m_Image.m_Image,
    vk::ImageLayout::eTransferDstOptimal,
    vkRegions);
}

void ezRHIVKCommandList::CopyTexturePlatform(const ezRHIResource* pSrcTexture, const ezRHIResource* pDstTexture, const ezDynamicArray<ezRHITextureCopyRegion>& regions)
{
  decltype(auto) vkSrcTexture = pSrcTexture->As<ezRHIVKResource>();
  decltype(auto) vkDstTexture = pDstTexture->As<ezRHIVKResource>();
  std::vector<vk::ImageCopy> vkRegions;
  for (const auto& region : regions)
  {
    auto& vk_region = vkRegions.emplace_back();
    vk_region.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    vk_region.srcSubresource.mipLevel = region.m_SrcMipLevel;
    vk_region.srcSubresource.baseArrayLayer = region.m_SrcArrayLayer;
    vk_region.srcSubresource.layerCount = 1;
    vk_region.srcOffset.x = region.m_SrcOffset.x;
    vk_region.srcOffset.y = region.m_SrcOffset.y;
    vk_region.srcOffset.z = region.m_SrcOffset.z;

    vk_region.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    vk_region.dstSubresource.mipLevel = region.m_DstMipLevel;
    vk_region.dstSubresource.baseArrayLayer = region.m_DstArrayLayer;
    vk_region.dstSubresource.layerCount = 1;
    vk_region.dstOffset.x = region.m_DstOffset.x;
    vk_region.dstOffset.y = region.m_DstOffset.y;
    vk_region.dstOffset.z = region.m_DstOffset.z;

    vk_region.extent.width = region.m_Extent.m_Width;
    vk_region.extent.height = region.m_Extent.m_Height;
    vk_region.extent.depth = region.m_Extent.m_Depth;
  }
  m_CommandList->copyImage(vkSrcTexture->m_Image.m_Image, vk::ImageLayout::eTransferSrcOptimal, vkDstTexture->m_Image.m_Image, vk::ImageLayout::eTransferDstOptimal, vkRegions);
}

void ezRHIVKCommandList::WriteAccelerationStructuresPropertiesPlatform(const ezDynamicArray<ezRHIResource*>& accelerationStructures, const ezRHIQueryHeap* pQueryHeap, ezUInt32 firstQuery)
{
  ezDynamicArray<vk::AccelerationStructureKHR> vkAccelerationStructures;
  vkAccelerationStructures.Reserve(accelerationStructures.GetCount());
  for (const auto& acceleration_structure : accelerationStructures)
  {
    vkAccelerationStructures.PushBack(acceleration_structure->As<ezRHIVKResource>()->m_AccelerationStructureHandle.get());
  }
  decltype(auto) vkQueryHeap = pQueryHeap->As<ezRHIVKQueryHeap>();
  auto queryType = vkQueryHeap->GetQueryType();
  EZ_ASSERT_ALWAYS(queryType == vk::QueryType::eAccelerationStructureCompactedSizeKHR, "");
  m_CommandList->resetQueryPool(vkQueryHeap->GetQueryPool(), firstQuery, accelerationStructures.GetCount());
  m_CommandList->writeAccelerationStructuresPropertiesKHR(
    vkAccelerationStructures.GetCount(),
    vkAccelerationStructures.GetData(),
    queryType,
    vkQueryHeap->GetQueryPool(),
    firstQuery);
}

void ezRHIVKCommandList::ResolveQueryDataPlatform(const ezRHIQueryHeap* pQueryHeap, ezUInt32 firstQuery, ezUInt32 queryCount, const ezRHIResource* pDstBuffer, ezUInt64 dstOffset)
{
  decltype(auto) vkQueryHeap = pQueryHeap->As<ezRHIVKQueryHeap>();
  auto queryType = vkQueryHeap->GetQueryType();
  EZ_ASSERT_ALWAYS(queryType == vk::QueryType::eAccelerationStructureCompactedSizeKHR, "");
  m_CommandList->copyQueryPoolResults(
    vkQueryHeap->GetQueryPool(),
    firstQuery,
    queryCount,
    pDstBuffer->As<ezRHIVKResource>()->m_Buffer.m_Buffer.get(),
    dstOffset,
    sizeof(uint64_t),
    vk::QueryResultFlagBits::eWait);
}

void ezRHIVKCommandList::BuildAccelerationStructure(vk::AccelerationStructureCreateInfoKHR& buildInfo, const vk::Buffer& instanceData, ezUInt64 instanceOffset, const ezRHIResource* src, const ezRHIResource* dst, const ezRHIResource* scratch, ezUInt64 scratchOffset)
{
}
