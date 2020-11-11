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



  struct DX12AllocationHandler
  {
    D3D12MA::Allocator* allocator = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Device> device;
    ezUInt64 framecount = 0;
    std::mutex destroylocker;
    std::deque<std::pair<D3D12MA::Allocation*, ezUInt64>> destroyer_allocations;
    std::deque<std::pair<Microsoft::WRL::ComPtr<ID3D12Resource>, ezUInt64>> destroyer_resources;
    std::deque<std::pair<ezUInt32, ezUInt64>> destroyer_queries_timestamp;
    std::deque<std::pair<ezUInt32, ezUInt64>> destroyer_queries_occlusion;
    std::deque<std::pair<Microsoft::WRL::ComPtr<ID3D12PipelineState>, ezUInt64>> destroyer_pipelines;
    std::deque<std::pair<Microsoft::WRL::ComPtr<ID3D12RootSignature>, ezUInt64>> destroyer_rootSignatures;
    std::deque<std::pair<Microsoft::WRL::ComPtr<ID3D12StateObject>, ezUInt64>> destroyer_stateobjects;
    std::deque<std::pair<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>, ezUInt64>> destroyer_descriptorHeaps;

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
      destroylocker.lock();
      framecount = FRAMECOUNT;
      while (!destroyer_allocations.empty())
      {
        if (destroyer_allocations.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_allocations.front();
          destroyer_allocations.pop_front();
          item.first->Release();
        }
        else
        {
          break;
        }
      }
      while (!destroyer_resources.empty())
      {
        if (destroyer_resources.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          destroyer_resources.pop_front();
          // comptr auto delete
        }
        else
        {
          break;
        }
      }
      while (!destroyer_queries_occlusion.empty())
      {
        if (destroyer_queries_occlusion.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_queries_occlusion.front();
          destroyer_queries_occlusion.pop_front();
          free_occlusionqueries.push_back(item.first);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_queries_timestamp.empty())
      {
        if (destroyer_queries_timestamp.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_queries_timestamp.front();
          destroyer_queries_timestamp.pop_front();
          free_timestampqueries.push_back(item.first);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_pipelines.empty())
      {
        if (destroyer_pipelines.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          destroyer_pipelines.pop_front();
          // comptr auto delete
        }
        else
        {
          break;
        }
      }
      while (!destroyer_rootSignatures.empty())
      {
        if (destroyer_rootSignatures.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          destroyer_rootSignatures.pop_front();
          // comptr auto delete
        }
        else
        {
          break;
        }
      }
      while (!destroyer_stateobjects.empty())
      {
        if (destroyer_stateobjects.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          destroyer_stateobjects.pop_front();
          // comptr auto delete
        }
        else
        {
          break;
        }
      }
      while (!destroyer_descriptorHeaps.empty())
      {
        if (destroyer_descriptorHeaps.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          destroyer_descriptorHeaps.pop_front();
          // comptr auto delete
        }
        else
        {
          break;
        }
      }
      destroylocker.unlock();
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


  struct Resource_DX12
  {
    std::shared_ptr<DX12AllocationHandler> allocationhandler;
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
      allocationhandler->destroylocker.lock();
      ezUInt64 framecount = allocationhandler->framecount;
      if (allocation)
        allocationhandler->destroyer_allocations.push_back(std::make_pair(allocation, framecount));
      if (resource)
        allocationhandler->destroyer_resources.push_back(std::make_pair(resource, framecount));
      allocationhandler->destroylocker.unlock();
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
      allocationhandler->destroylocker.lock();
      ezUInt64 framecount = allocationhandler->framecount;
      allocationhandler->destroylocker.unlock();
    }
  };
  struct Sampler_DX12
  {
    std::shared_ptr<DX12AllocationHandler> allocationhandler;
    D3D12_SAMPLER_DESC descriptor;

    ~Sampler_DX12()
    {
      allocationhandler->destroylocker.lock();
      ezUInt64 framecount = allocationhandler->framecount;
      allocationhandler->destroylocker.unlock();
    }
  };
  struct Query_DX12
  {
    std::shared_ptr<DX12AllocationHandler> allocationhandler;
    ezRHIGPUQueryType::Enum query_type = ezRHIGPUQueryType::GPU_QUERY_TYPE_INVALID;
    ezUInt32 query_index = ~0;

    ~Query_DX12()
    {
      if (query_index != ~0)
      {
        allocationhandler->destroylocker.lock();
        ezUInt64 framecount = allocationhandler->framecount;
        switch (query_type)
        {
          case ezRHIGPUQueryType::GPU_QUERY_TYPE_OCCLUSION:
          case ezRHIGPUQueryType::GPU_QUERY_TYPE_OCCLUSION_PREDICATE:
            allocationhandler->destroyer_queries_occlusion.push_back(std::make_pair(query_index, framecount));
            break;
          case ezRHIGPUQueryType::GPU_QUERY_TYPE_TIMESTAMP:
            allocationhandler->destroyer_queries_timestamp.push_back(std::make_pair(query_index, framecount));
            break;
        }
        allocationhandler->destroylocker.unlock();
      }
    }
  };
  struct PipelineState_DX12
  {
    std::shared_ptr<DX12AllocationHandler> allocationhandler;
    ComPtr<ID3D12PipelineState> resource;
    ComPtr<ID3D12RootSignature> rootSignature;

    ezDynamicArray<D3D12_DESCRIPTOR_RANGE> resources;
    ezDynamicArray<D3D12_DESCRIPTOR_RANGE> samplers;

    ~PipelineState_DX12()
    {
      allocationhandler->destroylocker.lock();
      ezUInt64 framecount = allocationhandler->framecount;
      if (resource)
        allocationhandler->destroyer_pipelines.push_back(std::make_pair(resource, framecount));
      if (rootSignature)
        allocationhandler->destroyer_rootSignatures.push_back(std::make_pair(rootSignature, framecount));
      allocationhandler->destroylocker.unlock();
    }
  };
  struct BVH_DX12 : public Resource_DX12
  {
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS desc = {};
    ezDynamicArray<D3D12_RAYTRACING_GEOMETRY_DESC> geometries;
    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info = {};
    GPUBuffer scratch;
  };
  struct RTPipelineState_DX12
  {
    std::shared_ptr<DX12AllocationHandler> allocationhandler;
    ComPtr<ID3D12StateObject> resource;

    ezDynamicArray<ezString> export_strings;
    ezDynamicArray<D3D12_EXPORT_DESC> exports;
    ezDynamicArray<D3D12_DXIL_LIBRARY_DESC> library_descs;
    ezDynamicArray<ezString> group_strings;
    ezDynamicArray<D3D12_HIT_GROUP_DESC> hitgroup_descs;

    ~RTPipelineState_DX12()
    {
      allocationhandler->destroylocker.lock();
      ezUInt64 framecount = allocationhandler->framecount;
      if (resource)
        allocationhandler->destroyer_stateobjects.push_back(std::make_pair(resource, framecount));
      allocationhandler->destroylocker.unlock();
    }
  };
  struct RenderPass_DX12
  {
    D3D12_RESOURCE_BARRIER barrierdescs_begin[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
    ezUInt32 num_barriers_begin = 0;
    D3D12_RESOURCE_BARRIER barrierdescs_end[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
    ezUInt32 num_barriers_end = 0;
  };
  struct DescriptorTable_DX12
  {
    std::shared_ptr<DX12AllocationHandler> allocationhandler;

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
      allocationhandler->destroylocker.lock();
      ezUInt64 framecount = allocationhandler->framecount;
      if (sampler_heap.heap)
        allocationhandler->destroyer_descriptorHeaps.push_back(std::make_pair(sampler_heap.heap, framecount));
      if (resource_heap.heap)
        allocationhandler->destroyer_descriptorHeaps.push_back(std::make_pair(resource_heap.heap, framecount));
      allocationhandler->destroylocker.unlock();
    }
  };
  struct RootSignature_DX12
  {
    std::shared_ptr<DX12AllocationHandler> allocationhandler;
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
      allocationhandler->destroylocker.lock();
      ezUInt64 framecount = allocationhandler->framecount;
      if (resource)
        allocationhandler->destroyer_rootSignatures.push_back(std::make_pair(resource, framecount));
      allocationhandler->destroylocker.unlock();
    }
  };

  Resource_DX12* to_internal(const GPUResource* param)
  {
    return static_cast<Resource_DX12*>(param->internal_state.get());
  }
  Resource_DX12* to_internal(const GPUBuffer* param)
  {
    return static_cast<Resource_DX12*>(param->internal_state.get());
  }
  Texture_DX12* to_internal(const Texture* param)
  {
    return static_cast<Texture_DX12*>(param->internal_state.get());
  }
  Sampler_DX12* to_internal(const Sampler* param)
  {
    return static_cast<Sampler_DX12*>(param->internal_state.get());
  }
  Query_DX12* to_internal(const GPUQuery* param)
  {
    return static_cast<Query_DX12*>(param->internal_state.get());
  }
  PipelineState_DX12* to_internal(const Shader* param)
  {
    return static_cast<PipelineState_DX12*>(param->internal_state.get());
  }
  PipelineState_DX12* to_internal(const PipelineState* param)
  {
    return static_cast<PipelineState_DX12*>(param->internal_state.get());
  }
  BVH_DX12* to_internal(const RaytracingAccelerationStructure* param)
  {
    return static_cast<BVH_DX12*>(param->internal_state.get());
  }
  RTPipelineState_DX12* to_internal(const RaytracingPipelineState* param)
  {
    return static_cast<RTPipelineState_DX12*>(param->internal_state.get());
  }
  RenderPass_DX12* to_internal(const RenderPass* param)
  {
    return static_cast<RenderPass_DX12*>(param->internal_state.get());
  }
  DescriptorTable_DX12* to_internal(const DescriptorTable* param)
  {
    return static_cast<DescriptorTable_DX12*>(param->internal_state.get());
  }
  RootSignature_DX12* to_internal(const RootSignature* param)
  {
    return static_cast<RootSignature_DX12*>(param->internal_state.get());
  }
} // namespace DX12_Internal
