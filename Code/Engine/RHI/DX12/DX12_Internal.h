#pragma once
#include <RHI/Descriptors/Descriptors.h>
#include <RHI/GraphicsDevice.h>
#include <RHI/RHIDLL.h>
#include <RHI/RHIInternal.h>
#include <RHI/RHIPCH.h>
#include <RHI/Resources/Resources.h>

#include <RHI/DX12/D3D12MemAlloc.h>
#include <RHI/DX12/d3dx12.h>

#include <d3d12shader.h>
#include <dxcapi.h>
#include <pix.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "Dxgi.lib")
#pragma comment(lib, "dxguid.lib")

#ifdef _WIN64
#  ifndef PLATFORM_UWP
#    pragma comment(lib, "dxcompiler.lib")
#  endif // PLATFORM_UWP
#endif   // _X64

#ifdef _DEBUG
#  include <d3d12sdklayers.h>
#endif // _DEBUG


#include <deque>
#include <mutex>
#include <unordered_map>

using namespace Microsoft::WRL;

namespace DX12_Internal
{
  static const size_t timestamp_query_count = 1024;
  static const size_t occlusion_query_count = 1024;



  struct DX12AllocationHandler : public ezRefCounted
  {
    D3D12MA::Allocator* allocator = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Device> device;
    ezUInt64 framecount = 0;
    ezMutex destroylocker;
    ezDeque<std::pair<D3D12MA::Allocation*, ezUInt64>> destroyer_allocations;
    ezDeque<std::pair<Microsoft::WRL::ComPtr<ID3D12Resource>, ezUInt64>> destroyer_resources;
    ezDeque<std::pair<ezUInt32, ezUInt64>> destroyer_queries_timestamp;
    ezDeque<std::pair<ezUInt32, ezUInt64>> destroyer_queries_occlusion;
    ezDeque<std::pair<Microsoft::WRL::ComPtr<ID3D12PipelineState>, ezUInt64>> destroyer_pipelines;
    ezDeque<std::pair<Microsoft::WRL::ComPtr<ID3D12RootSignature>, ezUInt64>> destroyer_rootSignatures;
    ezDeque<std::pair<Microsoft::WRL::ComPtr<ID3D12StateObject>, ezUInt64>> destroyer_stateobjects;
    ezDeque<std::pair<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>, ezUInt64>> destroyer_descriptorHeaps;

    ThreadSafeRingBuffer<ezUInt32, timestamp_query_count> free_timestampqueries;
    ThreadSafeRingBuffer<ezUInt32, occlusion_query_count> free_occlusionqueries;

    ~DX12AllocationHandler()
    {
      Update(~0, 0); // destroy all remaining
      if (allocator)
        allocator->Release();
    }

    // Deferred destroy of resources that the GPU is already finished with:
    void Update(ezUInt64 FRAMECOUNT, ezUInt32 BACKBUFFER_COUNT)
    {
      ezLock lock(destroylocker);
      framecount = FRAMECOUNT;
      while (!destroyer_allocations.IsEmpty())
      {
        if (destroyer_allocations.PeekFront().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_allocations.PeekFront();
          destroyer_allocations.PopFront();
          item.first->Release();
        }
        else
        {
          break;
        }
      }
      while (!destroyer_resources.IsEmpty())
      {
        if (destroyer_resources.PeekFront().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          destroyer_resources.PopFront();
          // comptr auto delete
        }
        else
        {
          break;
        }
      }
      while (!destroyer_queries_occlusion.IsEmpty())
      {
        if (destroyer_queries_occlusion.PeekFront().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_queries_occlusion.PeekFront();
          destroyer_queries_occlusion.PopFront();
          free_occlusionqueries.push_back(item.first);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_queries_timestamp.IsEmpty())
      {
        if (destroyer_queries_timestamp.PeekFront().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_queries_timestamp.PeekFront();
          destroyer_queries_timestamp.PopFront();
          free_timestampqueries.push_back(item.first);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_pipelines.IsEmpty())
      {
        if (destroyer_pipelines.PeekFront().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          destroyer_pipelines.PopFront();
          // comptr auto delete
        }
        else
        {
          break;
        }
      }
      while (!destroyer_rootSignatures.IsEmpty())
      {
        if (destroyer_rootSignatures.PeekFront().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          destroyer_rootSignatures.PopFront();
          // comptr auto delete
        }
        else
        {
          break;
        }
      }
      while (!destroyer_stateobjects.IsEmpty())
      {
        if (destroyer_stateobjects.PeekFront().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          destroyer_stateobjects.PopFront();
          // comptr auto delete
        }
        else
        {
          break;
        }
      }
      while (!destroyer_descriptorHeaps.IsEmpty())
      {
        if (destroyer_descriptorHeaps.PeekFront().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          destroyer_descriptorHeaps.PopFront();
          // comptr auto delete
        }
        else
        {
          break;
        }
      }
      destroylocker.Unlock();
    }
  };





  // Engine -> Native converters

  ezUInt32 _ParseColorWriteMask(ezBitflags<ezRHIColorWriteMask> value);

  inline D3D12_FILTER _ConvertFilter(ezEnum<ezRHIFilter> value);
  inline D3D12_TEXTURE_ADDRESS_MODE _ConvertTextureAddressMode(ezEnum<ezRHITextureAddressMode> value);
  inline D3D12_COMPARISON_FUNC _ConvertComparisonFunc(ezEnum<ezRHIComparisonFunc> value);
  inline D3D12_FILL_MODE _ConvertFillMode(ezEnum<ezRHIFillMode> value);
  inline D3D12_CULL_MODE _ConvertCullMode(ezEnum<ezRHICullMode> value);
  inline D3D12_DEPTH_WRITE_MASK _ConvertDepthWriteMask(ezEnum<ezRHIDepthWriteMask> value);
  inline D3D12_STENCIL_OP _ConvertStencilOp(ezEnum<ezRHIStencilOp> value);
  inline D3D12_BLEND _ConvertBlend(ezEnum<ezRHIBlendFactor> value);
  inline D3D12_BLEND_OP _ConvertBlendOp(ezEnum<ezRHIBlendOp> value);
  inline D3D12_INPUT_CLASSIFICATION _ConvertInputClassification(ezEnum<ezRHIInputClassification> value);
  inline DXGI_FORMAT _ConvertFormat(ezEnum<ezRHIFormat> value);
  inline D3D12_SUBRESOURCE_DATA _ConvertSubresourceData(const SubresourceData& pInitialData);
  constexpr D3D12_RESOURCE_STATES _ConvertImageLayout(IMAGE_LAYOUT value);
  constexpr D3D12_RESOURCE_STATES _ConvertBufferState(BUFFER_STATE value);

  inline D3D12_SHADER_VISIBILITY _ConvertShaderVisibility(ezEnum<ezRHIShaderStage> value);
  constexpr D3D12_SHADING_RATE _ConvertShadingRate(SHADING_RATE value);

  // Native -> Engine converters

  inline ezEnum<ezRHIFormat> _ConvertFormat_Inv(DXGI_FORMAT value);

  inline TextureDesc _ConvertTextureDesc_Inv(const D3D12_RESOURCE_DESC& desc);


  // Local Helpers:

  inline size_t Align(size_t uLocation, size_t uAlign)
  {
    if ((0 == uAlign) || (uAlign & (uAlign - 1)))
    {
      EZ_ASSERT_ALWAYS(false, "");
    }

    return ((uLocation + (uAlign - 1)) & ~(uAlign - 1));
  }


  struct Resource_DX12 : public ezRefCounted
  {
    ezSharedPtr<DX12AllocationHandler> allocationhandler;
    D3D12MA::Allocation* allocation = nullptr;
    ComPtr<ID3D12Resource> resource;
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbv = {};
    D3D12_SHADER_RESOURCE_VIEW_DESC srv = {};
    D3D12_UNORDERED_ACCESS_VIEW_DESC uav = {};
    ezDynamicArray<D3D12_SHADER_RESOURCE_VIEW_DESC> subresources_srv;
    ezDynamicArray<D3D12_UNORDERED_ACCESS_VIEW_DESC> subresources_uav;

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;

    GraphicsDevice::GPUAllocation dynamic[COMMANDLIST_COUNT];

    virtual ~Resource_DX12()
    {
      ezLock lock(allocationhandler->destroylocker);
      ezUInt64 framecount = allocationhandler->framecount;
      if (allocation)
        allocationhandler->destroyer_allocations.PushBack(std::make_pair(allocation, framecount));
      if (resource)
        allocationhandler->destroyer_resources.PushBack(std::make_pair(resource, framecount));
      allocationhandler->destroylocker.Unlock();
    }
  };
  struct Texture_DX12 : public Resource_DX12
  {
    D3D12_RENDER_TARGET_VIEW_DESC rtv = {};
    D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
    ezDynamicArray<D3D12_RENDER_TARGET_VIEW_DESC> subresources_rtv;
    ezDynamicArray<D3D12_DEPTH_STENCIL_VIEW_DESC> subresources_dsv;

    ~Texture_DX12() override
    {
      ezLock lock(allocationhandler->destroylocker);
      ezUInt64 framecount = allocationhandler->framecount;
      allocationhandler->destroylocker.Unlock();
    }
  };
  struct Sampler_DX12 : public ezRefCounted
  {
    ezSharedPtr<DX12AllocationHandler> allocationhandler;
    D3D12_SAMPLER_DESC descriptor;

    ~Sampler_DX12()
    {
      ezLock lock(allocationhandler->destroylocker);
      ezUInt64 framecount = allocationhandler->framecount;
      allocationhandler->destroylocker.Unlock();
    }
  };
  struct Query_DX12 : public ezRefCounted
  {
    ezSharedPtr<DX12AllocationHandler> allocationhandler;
    ezRHIGPUQueryType::Enum query_type = ezRHIGPUQueryType::GPU_QUERY_TYPE_INVALID;
    ezUInt32 query_index = ~0;

    ~Query_DX12()
    {
      if (query_index != ~0)
      {
        ezLock lock(allocationhandler->destroylocker);
        ezUInt64 framecount = allocationhandler->framecount;
        switch (query_type)
        {
          case ezRHIGPUQueryType::GPU_QUERY_TYPE_OCCLUSION:
          case ezRHIGPUQueryType::GPU_QUERY_TYPE_OCCLUSION_PREDICATE:
            allocationhandler->destroyer_queries_occlusion.PushBack(std::make_pair(query_index, framecount));
            break;
          case ezRHIGPUQueryType::GPU_QUERY_TYPE_TIMESTAMP:
            allocationhandler->destroyer_queries_timestamp.PushBack(std::make_pair(query_index, framecount));
            break;
        }
        allocationhandler->destroylocker.Unlock();
      }
    }
  };
  struct PipelineState_DX12 : public ezRefCounted
  {
    ezSharedPtr<DX12AllocationHandler> allocationhandler;
    ComPtr<ID3D12PipelineState> resource;
    ComPtr<ID3D12RootSignature> rootSignature;

    ezDynamicArray<D3D12_DESCRIPTOR_RANGE> resources;
    ezDynamicArray<D3D12_DESCRIPTOR_RANGE> samplers;

    ~PipelineState_DX12()
    {
      ezLock lock(allocationhandler->destroylocker);
      ezUInt64 framecount = allocationhandler->framecount;
      if (resource)
        allocationhandler->destroyer_pipelines.PushBack(std::make_pair(resource, framecount));
      if (rootSignature)
        allocationhandler->destroyer_rootSignatures.PushBack(std::make_pair(rootSignature, framecount));
      allocationhandler->destroylocker.Unlock();
    }
  };
  struct BVH_DX12 : public Resource_DX12
  {
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS desc = {};
    ezDynamicArray<D3D12_RAYTRACING_GEOMETRY_DESC> geometries;
    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info = {};
    GPUBuffer scratch;
  };
  struct RTPipelineState_DX12 : public ezRefCounted
  {
    ezSharedPtr<DX12AllocationHandler> allocationhandler;
    ComPtr<ID3D12StateObject> resource;

    ezDynamicArray<ezString> export_strings;
    ezDynamicArray<D3D12_EXPORT_DESC> exports;
    ezDynamicArray<D3D12_DXIL_LIBRARY_DESC> library_descs;
    ezDynamicArray<ezString> group_strings;
    ezDynamicArray<D3D12_HIT_GROUP_DESC> hitgroup_descs;

    ~RTPipelineState_DX12()
    {
      ezLock lock(allocationhandler->destroylocker);
      ezUInt64 framecount = allocationhandler->framecount;
      if (resource)
        allocationhandler->destroyer_stateobjects.PushBack(std::make_pair(resource, framecount));
      allocationhandler->destroylocker.Unlock();
    }
  };
  struct RenderPass_DX12 : public ezRefCounted
  {
    D3D12_RESOURCE_BARRIER barrierdescs_begin[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
    ezUInt32 num_barriers_begin = 0;
    D3D12_RESOURCE_BARRIER barrierdescs_end[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
    ezUInt32 num_barriers_end = 0;
  };
  struct DescriptorTable_DX12 : public ezRefCounted
  {
    ezSharedPtr<DX12AllocationHandler> allocationhandler;

    struct Heap
    {
      ComPtr<ID3D12DescriptorHeap> heap;
      D3D12_DESCRIPTOR_HEAP_DESC desc = {};
      D3D12_CPU_DESCRIPTOR_HANDLE address = {};
      ezDynamicArray<D3D12_DESCRIPTOR_RANGE> ranges;
      ezDynamicArray<size_t> write_remap;
    };
    Heap sampler_heap;
    Heap resource_heap;
    ezDynamicArray<D3D12_STATIC_SAMPLER_DESC> staticsamplers;

    ~DescriptorTable_DX12()
    {
      ezLock lock(allocationhandler->destroylocker);
      ezUInt64 framecount = allocationhandler->framecount;
      if (sampler_heap.heap)
        allocationhandler->destroyer_descriptorHeaps.PushBack(std::make_pair(sampler_heap.heap, framecount));
      if (resource_heap.heap)
        allocationhandler->destroyer_descriptorHeaps.PushBack(std::make_pair(resource_heap.heap, framecount));
      allocationhandler->destroylocker.Unlock();
    }
  };
  struct RootSignature_DX12 : public ezRefCounted
  {
    ezSharedPtr<DX12AllocationHandler> allocationhandler;
    ComPtr<ID3D12RootSignature> resource;
    ezDynamicArray<D3D12_ROOT_PARAMETER> params;

    ezDynamicArray<ezUInt32> table_bind_point_remap;
    ezUInt32 root_constant_bind_remap = 0;

    struct RootRemap
    {
      ezUInt32 space = 0;
      ezUInt32 rangeIndex = 0;
    };
    ezDynamicArray<RootRemap> root_remap;

    ~RootSignature_DX12()
    {
      ezLock lock(allocationhandler->destroylocker);
      ezUInt64 framecount = allocationhandler->framecount;
      if (resource)
        allocationhandler->destroyer_rootSignatures.PushBack(std::make_pair(resource, framecount));
      allocationhandler->destroylocker.Unlock();
    }
  };

  Resource_DX12* to_internal(const GPUResource* param)
  {
    return static_cast<Resource_DX12*>(param->internal_state.Borrow());
  }
  Resource_DX12* to_internal(const GPUBuffer* param)
  {
    return static_cast<Resource_DX12*>(param->internal_state.Borrow());
  }
  Texture_DX12* to_internal(const Texture* param)
  {
    return static_cast<Texture_DX12*>(param->internal_state.Borrow());
  }
  Sampler_DX12* to_internal(const Sampler* param)
  {
    return static_cast<Sampler_DX12*>(param->internal_state.Borrow());
  }
  Query_DX12* to_internal(const GPUQuery* param)
  {
    return static_cast<Query_DX12*>(param->internal_state.Borrow());
  }
  PipelineState_DX12* to_internal(const Shader* param)
  {
    return static_cast<PipelineState_DX12*>(param->internal_state.Borrow());
  }
  PipelineState_DX12* to_internal(const PipelineState* param)
  {
    return static_cast<PipelineState_DX12*>(param->internal_state.Borrow());
  }
  BVH_DX12* to_internal(const RaytracingAccelerationStructure* param)
  {
    return static_cast<BVH_DX12*>(param->internal_state.Borrow());
  }
  RTPipelineState_DX12* to_internal(const RaytracingPipelineState* param)
  {
    return static_cast<RTPipelineState_DX12*>(param->internal_state.Borrow());
  }
  RenderPass_DX12* to_internal(const RenderPass* param)
  {
    return static_cast<RenderPass_DX12*>(param->internal_state.Borrow());
  }
  DescriptorTable_DX12* to_internal(const DescriptorTable* param)
  {
    return static_cast<DescriptorTable_DX12*>(param->internal_state.Borrow());
  }
  RootSignature_DX12* to_internal(const RootSignature* param)
  {
    return static_cast<RootSignature_DX12*>(param->internal_state.Borrow());
  }
} // namespace DX12_Internal
