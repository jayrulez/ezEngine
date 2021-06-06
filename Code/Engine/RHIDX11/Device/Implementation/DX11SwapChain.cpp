#include <RHIDX11/Device/DX11SwapChain.h>
#include <RHIDX11/RHIDX11Utils.h>
#include <Core/System/Window.h>
#include <Foundation/Basics/Platform/Win/IncludeWindows.h>

using namespace Microsoft::WRL;

ezRHIDX11SwapChain::ezRHIDX11SwapChain(const ezRHISwapChainDesc& desc)
  : ezRHISwapChain{desc}
{
}

ezRHIDX11SwapChain::~ezRHIDX11SwapChain()
{
}

ezResult ezRHIDX11SwapChain::InitPlatform(ezRHIDevice* pDevice)
{
  HRESULT hr;
  ezRHIDX11Device* pDXDevice = static_cast<ezRHIDX11Device*>(pDevice);

  // Create swapchain:
  DXGI_SWAP_CHAIN_DESC1 sd = {};
  sd.Width = m_Desc.Width;
  sd.Height = m_Desc.Height;
  sd.Format = ezRHIDX11Utils::_ConvertFormat(m_Desc.Format);
  sd.Stereo = false;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.BufferCount = m_Desc.BufferCount;
  sd.Flags = 0;
  sd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
  sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

#if EZ_ENABLED(EZ_PLATFORM_WINDOWS_UWP)
  sd.Scaling = DXGI_SCALING_ASPECT_RATIO_STRETCH;

  hr = pDXDevice->GetDXGIFactory()->CreateSwapChainForCoreWindow(
    pDXDevice->GetD3D11Device().Get(),
    static_cast<IUnknown*>(winrt::get_abi(*ezMinWindows::ToNative(m_Desc.Window->GetNativeWindowHandle()))),
    &sd,
    nullptr,
    &m_pSwapChain);
#else
  sd.Scaling = DXGI_SCALING_STRETCH;

  DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc;
  fullscreenDesc.RefreshRate.Numerator = 60;
  fullscreenDesc.RefreshRate.Denominator = 1;
  fullscreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // needs to be unspecified for correct fullscreen scaling!
  fullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
  fullscreenDesc.Windowed = !m_Desc.FullScreen;
  hr = pDXDevice->GetDXGIFactory()->CreateSwapChainForHwnd(
    pDXDevice->GetD3D11Device().Get(),
    ezMinWindows::ToNative(m_Desc.Window->GetNativeWindowHandle()),
    &sd,
    &fullscreenDesc,
    nullptr,
    &m_pSwapChain);
#endif
  return ezResult();
}

ezResult ezRHIDX11SwapChain::DeInitPlatform(ezRHIDevice* pDevice)
{
  return ezResult();
}
