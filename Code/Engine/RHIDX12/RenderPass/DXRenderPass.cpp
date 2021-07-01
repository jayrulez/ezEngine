#pragma once
#include <RHIDX12/RenderPass/DXRenderPass.h>

DXRenderPass::DXRenderPass(DXDevice& device, const RenderPassDesc& desc)
    : m_desc(desc)
{
}

const RenderPassDesc& DXRenderPass::GetDesc() const
{
    return m_desc;
}
