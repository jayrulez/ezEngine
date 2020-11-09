/*
 * Copyright 2016-2020 The Brenwill Workshop Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SPIRV_CROSS_MSL_HPP
#define SPIRV_CROSS_MSL_HPP

#include <RHI/Utility/spirv_glsl.hpp>
#include <map>
#include <set>
#include <stddef.h>
#include <unordered_map>
#include <unordered_set>

namespace SPIRV_CROSS_NAMESPACE
{

// Indicates the format of a shader input. Currently limited to specifying
// if the input is an 8-bit unsigned integer, 16-bit unsigned integer, or
// some other format.
enum MSLShaderInputFormat
{
	MSL_SHADER_INPUT_FORMAT_OTHER = 0,
	MSL_SHADER_INPUT_FORMAT_UINT8 = 1,
	MSL_SHADER_INPUT_FORMAT_UINT16 = 2,

	// Deprecated aliases.
	MSL_VERTEX_FORMAT_OTHER = MSL_SHADER_INPUT_FORMAT_OTHER,
	MSL_VERTEX_FORMAT_UINT8 = MSL_SHADER_INPUT_FORMAT_UINT8,
	MSL_VERTEX_FORMAT_UINT16 = MSL_SHADER_INPUT_FORMAT_UINT16,

	MSL_SHADER_INPUT_FORMAT_INT_MAX = 0x7fffffff
};

// Defines MSL characteristics of an input variable at a particular location.
// After compilation, it is possible to query whether or not this location was used.
// If vecsize is nonzero, it must be greater than or equal to the vecsize declared in the shader,
// or behavior is undefined.
struct MSLShaderInput
{
	ezUInt32 location = 0;
	MSLShaderInputFormat format = MSL_SHADER_INPUT_FORMAT_OTHER;
	spv::BuiltIn builtin = spv::BuiltInMax;
	ezUInt32 vecsize = 0;
};

// Matches the binding index of a MSL resource for a binding within a descriptor set.
// Taken together, the stage, desc_set and binding combine to form a reference to a resource
// descriptor used in a particular shading stage.
// If using MSL 2.0 argument buffers, the descriptor set is not marked as a discrete descriptor set,
// and (for iOS only) the resource is not a storage image (sampled != 2), the binding reference we
// remap to will become an [[id(N)]] attribute within the "descriptor set" argument buffer structure.
// For resources which are bound in the "classic" MSL 1.0 way or discrete descriptors, the remap will become a
// [[buffer(N)]], [[texture(N)]] or [[sampler(N)]] depending on the resource types used.
struct MSLResourceBinding
{
	spv::ExecutionModel stage = spv::ExecutionModelMax;
	ezUInt32 desc_set = 0;
	ezUInt32 binding = 0;
	ezUInt32 msl_buffer = 0;
	ezUInt32 msl_texture = 0;
	ezUInt32 msl_sampler = 0;
};

enum MSLSamplerCoord
{
	MSL_SAMPLER_COORD_NORMALIZED = 0,
	MSL_SAMPLER_COORD_PIXEL = 1,
	MSL_SAMPLER_INT_MAX = 0x7fffffff
};

enum MSLSamplerFilter
{
	MSL_SAMPLER_FILTER_NEAREST = 0,
	MSL_SAMPLER_FILTER_LINEAR = 1,
	MSL_SAMPLER_FILTER_INT_MAX = 0x7fffffff
};

enum MSLSamplerMipFilter
{
	MSL_SAMPLER_MIP_FILTER_NONE = 0,
	MSL_SAMPLER_MIP_FILTER_NEAREST = 1,
	MSL_SAMPLER_MIP_FILTER_LINEAR = 2,
	MSL_SAMPLER_MIP_FILTER_INT_MAX = 0x7fffffff
};

enum MSLSamplerAddress
{
	MSL_SAMPLER_ADDRESS_CLAMP_TO_ZERO = 0,
	MSL_SAMPLER_ADDRESS_CLAMP_TO_EDGE = 1,
	MSL_SAMPLER_ADDRESS_CLAMP_TO_BORDER = 2,
	MSL_SAMPLER_ADDRESS_REPEAT = 3,
	MSL_SAMPLER_ADDRESS_MIRRORED_REPEAT = 4,
	MSL_SAMPLER_ADDRESS_INT_MAX = 0x7fffffff
};

enum MSLSamplerCompareFunc
{
	MSL_SAMPLER_COMPARE_FUNC_NEVER = 0,
	MSL_SAMPLER_COMPARE_FUNC_LESS = 1,
	MSL_SAMPLER_COMPARE_FUNC_LESS_EQUAL = 2,
	MSL_SAMPLER_COMPARE_FUNC_GREATER = 3,
	MSL_SAMPLER_COMPARE_FUNC_GREATER_EQUAL = 4,
	MSL_SAMPLER_COMPARE_FUNC_EQUAL = 5,
	MSL_SAMPLER_COMPARE_FUNC_NOT_EQUAL = 6,
	MSL_SAMPLER_COMPARE_FUNC_ALWAYS = 7,
	MSL_SAMPLER_COMPARE_FUNC_INT_MAX = 0x7fffffff
};

enum MSLSamplerBorderColor
{
	MSL_SAMPLER_BORDER_COLOR_TRANSPARENT_BLACK = 0,
	MSL_SAMPLER_BORDER_COLOR_OPAQUE_BLACK = 1,
	MSL_SAMPLER_BORDER_COLOR_OPAQUE_WHITE = 2,
	MSL_SAMPLER_BORDER_COLOR_INT_MAX = 0x7fffffff
};

enum MSLFormatResolution
{
	MSL_FORMAT_RESOLUTION_444 = 0,
	MSL_FORMAT_RESOLUTION_422,
	MSL_FORMAT_RESOLUTION_420,
	MSL_FORMAT_RESOLUTION_INT_MAX = 0x7fffffff
};

enum MSLChromaLocation
{
	MSL_CHROMA_LOCATION_COSITED_EVEN = 0,
	MSL_CHROMA_LOCATION_MIDPOINT,
	MSL_CHROMA_LOCATION_INT_MAX = 0x7fffffff
};

enum MSLComponentSwizzle
{
	MSL_COMPONENT_SWIZZLE_IDENTITY = 0,
	MSL_COMPONENT_SWIZZLE_ZERO,
	MSL_COMPONENT_SWIZZLE_ONE,
	MSL_COMPONENT_SWIZZLE_R,
	MSL_COMPONENT_SWIZZLE_G,
	MSL_COMPONENT_SWIZZLE_B,
	MSL_COMPONENT_SWIZZLE_A,
	MSL_COMPONENT_SWIZZLE_INT_MAX = 0x7fffffff
};

enum MSLSamplerYCbCrModelConversion
{
	MSL_SAMPLER_YCBCR_MODEL_CONVERSION_RGB_IDENTITY = 0,
	MSL_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_IDENTITY,
	MSL_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_BT_709,
	MSL_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_BT_601,
	MSL_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_BT_2020,
	MSL_SAMPLER_YCBCR_MODEL_CONVERSION_INT_MAX = 0x7fffffff
};

enum MSLSamplerYCbCrRange
{
	MSL_SAMPLER_YCBCR_RANGE_ITU_FULL = 0,
	MSL_SAMPLER_YCBCR_RANGE_ITU_NARROW,
	MSL_SAMPLER_YCBCR_RANGE_INT_MAX = 0x7fffffff
};

struct MSLConstexprSampler
{
	MSLSamplerCoord coord = MSL_SAMPLER_COORD_NORMALIZED;
	MSLSamplerFilter min_filter = MSL_SAMPLER_FILTER_NEAREST;
	MSLSamplerFilter mag_filter = MSL_SAMPLER_FILTER_NEAREST;
	MSLSamplerMipFilter mip_filter = MSL_SAMPLER_MIP_FILTER_NONE;
	MSLSamplerAddress s_address = MSL_SAMPLER_ADDRESS_CLAMP_TO_EDGE;
	MSLSamplerAddress t_address = MSL_SAMPLER_ADDRESS_CLAMP_TO_EDGE;
	MSLSamplerAddress r_address = MSL_SAMPLER_ADDRESS_CLAMP_TO_EDGE;
	MSLSamplerCompareFunc compare_func = MSL_SAMPLER_COMPARE_FUNC_NEVER;
	MSLSamplerBorderColor border_color = MSL_SAMPLER_BORDER_COLOR_TRANSPARENT_BLACK;
	float lod_clamp_min = 0.0f;
	float lod_clamp_max = 1000.0f;
	int max_anisotropy = 1;

	// Sampler Y'CbCr conversion parameters
	ezUInt32 planes = 0;
	MSLFormatResolution resolution = MSL_FORMAT_RESOLUTION_444;
	MSLSamplerFilter chroma_filter = MSL_SAMPLER_FILTER_NEAREST;
	MSLChromaLocation x_chroma_offset = MSL_CHROMA_LOCATION_COSITED_EVEN;
	MSLChromaLocation y_chroma_offset = MSL_CHROMA_LOCATION_COSITED_EVEN;
	MSLComponentSwizzle swizzle[4]; // IDENTITY, IDENTITY, IDENTITY, IDENTITY
	MSLSamplerYCbCrModelConversion ycbcr_model = MSL_SAMPLER_YCBCR_MODEL_CONVERSION_RGB_IDENTITY;
	MSLSamplerYCbCrRange ycbcr_range = MSL_SAMPLER_YCBCR_RANGE_ITU_FULL;
	ezUInt32 bpc = 8;

	bool compare_enable = false;
	bool lod_clamp_enable = false;
	bool anisotropy_enable = false;
	bool ycbcr_conversion_enable = false;

	MSLConstexprSampler()
	{
		for (ezUInt32 i = 0; i < 4; i++)
			swizzle[i] = MSL_COMPONENT_SWIZZLE_IDENTITY;
	}
	bool swizzle_is_identity() const
	{
		return (swizzle[0] == MSL_COMPONENT_SWIZZLE_IDENTITY && swizzle[1] == MSL_COMPONENT_SWIZZLE_IDENTITY &&
		        swizzle[2] == MSL_COMPONENT_SWIZZLE_IDENTITY && swizzle[3] == MSL_COMPONENT_SWIZZLE_IDENTITY);
	}
	bool swizzle_has_one_or_zero() const
	{
		return (swizzle[0] == MSL_COMPONENT_SWIZZLE_ZERO || swizzle[0] == MSL_COMPONENT_SWIZZLE_ONE ||
		        swizzle[1] == MSL_COMPONENT_SWIZZLE_ZERO || swizzle[1] == MSL_COMPONENT_SWIZZLE_ONE ||
		        swizzle[2] == MSL_COMPONENT_SWIZZLE_ZERO || swizzle[2] == MSL_COMPONENT_SWIZZLE_ONE ||
		        swizzle[3] == MSL_COMPONENT_SWIZZLE_ZERO || swizzle[3] == MSL_COMPONENT_SWIZZLE_ONE);
	}
};

// Special constant used in a MSLResourceBinding desc_set
// element to indicate the bindings for the push constants.
// Kinda deprecated. Just use ResourceBindingPushConstant{DescriptorSet,Binding} directly.
static const ezUInt32 kPushConstDescSet = ResourceBindingPushConstantDescriptorSet;

// Special constant used in a MSLResourceBinding binding
// element to indicate the bindings for the push constants.
// Kinda deprecated. Just use ResourceBindingPushConstant{DescriptorSet,Binding} directly.
static const ezUInt32 kPushConstBinding = ResourceBindingPushConstantBinding;

// Special constant used in a MSLResourceBinding binding
// element to indicate the buffer binding for swizzle buffers.
static const ezUInt32 kSwizzleBufferBinding = ~(1u);

// Special constant used in a MSLResourceBinding binding
// element to indicate the buffer binding for buffer size buffers to support OpArrayLength.
static const ezUInt32 kBufferSizeBufferBinding = ~(2u);

// Special constant used in a MSLResourceBinding binding
// element to indicate the buffer binding used for the argument buffer itself.
// This buffer binding should be kept as small as possible as all automatic bindings for buffers
// will start at max(kArgumentBufferBinding) + 1.
static const ezUInt32 kArgumentBufferBinding = ~(3u);

static const ezUInt32 kMaxArgumentBuffers = 8;

// Decompiles SPIR-V to Metal Shading Language
class CompilerMSL : public CompilerGLSL
{
public:
	// Options for compiling to Metal Shading Language
	struct Options
	{
		typedef enum
		{
			iOS = 0,
			macOS = 1
		} Platform;

		Platform platform = macOS;
		ezUInt32 msl_version = make_msl_version(1, 2);
		ezUInt32 texel_buffer_texture_width = 4096; // Width of 2D Metal textures used as 1D texel buffers
		ezUInt32 swizzle_buffer_index = 30;
		ezUInt32 indirect_params_buffer_index = 29;
		ezUInt32 shader_output_buffer_index = 28;
		ezUInt32 shader_patch_output_buffer_index = 27;
		ezUInt32 shader_tess_factor_buffer_index = 26;
		ezUInt32 buffer_size_buffer_index = 25;
		ezUInt32 view_mask_buffer_index = 24;
		ezUInt32 dynamic_offsets_buffer_index = 23;
		ezUInt32 shader_input_wg_index = 0;
		ezUInt32 device_index = 0;
		ezUInt32 enable_frag_output_mask = 0xffffffff;
		bool enable_point_size_builtin = true;
		bool enable_frag_depth_builtin = true;
		bool enable_frag_stencil_ref_builtin = true;
		bool disable_rasterization = false;
		bool capture_output_to_buffer = false;
		bool swizzle_texture_samples = false;
		bool tess_domain_origin_lower_left = false;
		bool multiview = false;
		bool view_index_from_device_index = false;
		bool dispatch_base = false;
		bool texture_1D_as_2D = false;

		// Enable use of MSL 2.0 indirect argument buffers.
		// MSL 2.0 must also be enabled.
		bool argument_buffers = false;

		// Ensures vertex and instance indices start at zero. This reflects the behavior of HLSL with SV_VertexID and SV_InstanceID.
		bool enable_base_index_zero = false;

		// Fragment output in MSL must have at least as many components as the render pass.
		// Add support to explicit pad out components.
		bool pad_fragment_output_components = false;

		// Specifies whether the iOS target version supports the [[base_vertex]] and [[base_instance]] attributes.
		bool ios_support_base_vertex_instance = false;

		// Use Metal's native frame-buffer fetch API for subpass inputs.
		bool ios_use_framebuffer_fetch_subpasses = false;

		// Enables use of "fma" intrinsic for invariant float math
		bool invariant_float_math = false;

		// Emulate texturecube_array with texture2d_array for iOS where this type is not available
		bool emulate_cube_array = false;

		// Allow user to enable decoration binding
		bool enable_decoration_binding = false;

		// Requires MSL 2.1, use the native support for texel buffers.
		bool texture_buffer_native = false;

		// Forces all resources which are part of an argument buffer to be considered active.
		// This ensures ABI compatibility between shaders where some resources might be unused,
		// and would otherwise declare a different IAB.
		bool force_active_argument_buffer_resources = false;

		// Forces the use of plain arrays, which works around certain driver bugs on certain versions
		// of Intel Macbooks. See https://github.com/KhronosGroup/SPIRV-Cross/issues/1210.
		// May reduce performance in scenarios where arrays are copied around as value-types.
		bool force_native_arrays = false;

		// If a shader writes clip distance, also emit user varyings which
		// can be read in subsequent stages.
		bool enable_clip_distance_user_varying = true;

		bool is_ios() const
		{
			return platform == iOS;
		}

		bool is_macos() const
		{
			return platform == macOS;
		}

		void set_msl_version(ezUInt32 major, ezUInt32 minor = 0, ezUInt32 patch = 0)
		{
			msl_version = make_msl_version(major, minor, patch);
		}

		bool supports_msl_version(ezUInt32 major, ezUInt32 minor = 0, ezUInt32 patch = 0) const
		{
			return msl_version >= make_msl_version(major, minor, patch);
		}

		static ezUInt32 make_msl_version(ezUInt32 major, ezUInt32 minor = 0, ezUInt32 patch = 0)
		{
			return (major * 10000) + (minor * 100) + patch;
		}
	};

	const Options &get_msl_options() const
	{
		return msl_options;
	}

	void set_msl_options(const Options &opts)
	{
		msl_options = opts;
	}

	// Provide feedback to calling API to allow runtime to disable pipeline
	// rasterization if vertex shader requires rasterization to be disabled.
	bool get_is_rasterization_disabled() const
	{
		return is_rasterization_disabled && (get_entry_point().model == spv::ExecutionModelVertex ||
		                                     get_entry_point().model == spv::ExecutionModelTessellationControl ||
		                                     get_entry_point().model == spv::ExecutionModelTessellationEvaluation);
	}

	// Provide feedback to calling API to allow it to pass an auxiliary
	// swizzle buffer if the shader needs it.
	bool needs_swizzle_buffer() const
	{
		return used_swizzle_buffer;
	}

	// Provide feedback to calling API to allow it to pass a buffer
	// containing STORAGE_BUFFER buffer sizes to support OpArrayLength.
	bool needs_buffer_size_buffer() const
	{
		return !buffers_requiring_array_length.empty();
	}

	// Provide feedback to calling API to allow it to pass a buffer
	// containing the view mask for the current multiview subpass.
	bool needs_view_mask_buffer() const
	{
		return msl_options.multiview && !msl_options.view_index_from_device_index;
	}

	// Provide feedback to calling API to allow it to pass a buffer
	// containing the dispatch base workgroup ID.
	bool needs_dispatch_base_buffer() const
	{
		return msl_options.dispatch_base && !msl_options.supports_msl_version(1, 2);
	}

	// Provide feedback to calling API to allow it to pass an output
	// buffer if the shader needs it.
	bool needs_output_buffer() const
	{
		return capture_output_to_buffer && stage_out_var_id != ID(0);
	}

	// Provide feedback to calling API to allow it to pass a patch output
	// buffer if the shader needs it.
	bool needs_patch_output_buffer() const
	{
		return capture_output_to_buffer && patch_stage_out_var_id != ID(0);
	}

	// Provide feedback to calling API to allow it to pass an input threadgroup
	// buffer if the shader needs it.
	bool needs_input_threadgroup_mem() const
	{
		return capture_output_to_buffer && stage_in_var_id != ID(0);
	}

	explicit CompilerMSL(std::vector<ezUInt32> spirv);
	CompilerMSL(const ezUInt32 *ir, size_t word_count);
	explicit CompilerMSL(const ParsedIR &ir);
	explicit CompilerMSL(ParsedIR &&ir);

	// input is a shader input description used to fix up shader input variables.
	// If shader inputs are provided, is_msl_shader_input_used() will return true after
	// calling ::compile() if the location was used by the MSL code.
	void add_msl_shader_input(const MSLShaderInput &attr);

	// resource is a resource binding to indicate the MSL buffer,
	// texture or sampler index to use for a particular SPIR-V description set
	// and binding. If resource bindings are provided,
	// is_msl_resource_binding_used() will return true after calling ::compile() if
	// the set/binding combination was used by the MSL code.
	void add_msl_resource_binding(const MSLResourceBinding &resource);

	// desc_set and binding are the SPIR-V descriptor set and binding of a buffer resource
	// in this shader. index is the index within the dynamic offset buffer to use. This
	// function marks that resource as using a dynamic offset (VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
	// or VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC). This function only has any effect if argument buffers
	// are enabled. If so, the buffer will have its address adjusted at the beginning of the shader with
	// an offset taken from the dynamic offset buffer.
	void add_dynamic_buffer(ezUInt32 desc_set, ezUInt32 binding, ezUInt32 index);

	// desc_set and binding are the SPIR-V descriptor set and binding of a buffer resource
	// in this shader. This function marks that resource as an inline uniform block
	// (VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT). This function only has any effect if argument buffers
	// are enabled. If so, the buffer block will be directly embedded into the argument
	// buffer, instead of being referenced indirectly via pointer.
	void add_inline_uniform_block(ezUInt32 desc_set, ezUInt32 binding);

	// When using MSL argument buffers, we can force "classic" MSL 1.0 binding schemes for certain descriptor sets.
	// This corresponds to VK_KHR_push_descriptor in Vulkan.
	void add_discrete_descriptor_set(ezUInt32 desc_set);

	// If an argument buffer is large enough, it may need to be in the device storage space rather than
	// constant. Opt-in to this behavior here on a per set basis.
	void set_argument_buffer_device_address_space(ezUInt32 desc_set, bool device_storage);

	// Query after compilation is done. This allows you to check if an input location was used by the shader.
	bool is_msl_shader_input_used(ezUInt32 location);

	// NOTE: Only resources which are remapped using add_msl_resource_binding will be reported here.
	// Constexpr samplers are always assumed to be emitted.
	// No specific MSLResourceBinding remapping is required for constexpr samplers as long as they are remapped
	// by remap_constexpr_sampler(_by_binding).
	bool is_msl_resource_binding_used(spv::ExecutionModel model, ezUInt32 set, ezUInt32 binding) const;

	// This must only be called after a successful call to CompilerMSL::compile().
	// For a variable resource ID obtained through reflection API, report the automatically assigned resource index.
	// If the descriptor set was part of an argument buffer, report the [[id(N)]],
	// or [[buffer/texture/sampler]] binding for other resources.
	// If the resource was a combined image sampler, report the image binding here,
	// use the _secondary version of this call to query the sampler half of the resource.
	// If no binding exists, ezUInt32(-1) is returned.
	ezUInt32 get_automatic_msl_resource_binding(ezUInt32 id) const;

	// Same as get_automatic_msl_resource_binding, but should only be used for combined image samplers, in which case the
	// sampler's binding is returned instead. For any other resource type, -1 is returned.
	ezUInt32 get_automatic_msl_resource_binding_secondary(ezUInt32 id) const;

	// Same as get_automatic_msl_resource_binding, but should only be used for combined image samplers for multiplanar images,
	// in which case the second plane's binding is returned instead. For any other resource type, -1 is returned.
	ezUInt32 get_automatic_msl_resource_binding_tertiary(ezUInt32 id) const;

	// Same as get_automatic_msl_resource_binding, but should only be used for combined image samplers for triplanar images,
	// in which case the third plane's binding is returned instead. For any other resource type, -1 is returned.
	ezUInt32 get_automatic_msl_resource_binding_quaternary(ezUInt32 id) const;

	// Compiles the SPIR-V code into Metal Shading Language.
	std::string compile() override;

	// Remap a sampler with ID to a constexpr sampler.
	// Older iOS targets must use constexpr samplers in certain cases (PCF),
	// so a static sampler must be used.
	// The sampler will not consume a binding, but be declared in the entry point as a constexpr sampler.
	// This can be used on both combined image/samplers (sampler2D) or standalone samplers.
	// The remapped sampler must not be an array of samplers.
	// Prefer remap_constexpr_sampler_by_binding unless you're also doing reflection anyways.
	void remap_constexpr_sampler(VariableID id, const MSLConstexprSampler &sampler);

	// Same as remap_constexpr_sampler, except you provide set/binding, rather than variable ID.
	// Remaps based on ID take priority over set/binding remaps.
	void remap_constexpr_sampler_by_binding(ezUInt32 desc_set, ezUInt32 binding, const MSLConstexprSampler &sampler);

	// If using CompilerMSL::Options::pad_fragment_output_components, override the number of components we expect
	// to use for a particular location. The default is 4 if number of components is not overridden.
	void set_fragment_output_components(ezUInt32 location, ezUInt32 components);

protected:
	// An enum of SPIR-V functions that are implemented in additional
	// source code that is added to the shader if necessary.
	enum SPVFuncImpl
	{
		SPVFuncImplNone,
		SPVFuncImplMod,
		SPVFuncImplRadians,
		SPVFuncImplDegrees,
		SPVFuncImplFindILsb,
		SPVFuncImplFindSMsb,
		SPVFuncImplFindUMsb,
		SPVFuncImplSSign,
		SPVFuncImplArrayCopyMultidimBase,
		// Unfortunately, we cannot use recursive templates in the MSL compiler properly,
		// so stamp out variants up to some arbitrary maximum.
		SPVFuncImplArrayCopy = SPVFuncImplArrayCopyMultidimBase + 1,
		SPVFuncImplArrayOfArrayCopy2Dim = SPVFuncImplArrayCopyMultidimBase + 2,
		SPVFuncImplArrayOfArrayCopy3Dim = SPVFuncImplArrayCopyMultidimBase + 3,
		SPVFuncImplArrayOfArrayCopy4Dim = SPVFuncImplArrayCopyMultidimBase + 4,
		SPVFuncImplArrayOfArrayCopy5Dim = SPVFuncImplArrayCopyMultidimBase + 5,
		SPVFuncImplArrayOfArrayCopy6Dim = SPVFuncImplArrayCopyMultidimBase + 6,
		SPVFuncImplTexelBufferCoords,
		SPVFuncImplImage2DAtomicCoords, // Emulate texture2D atomic operations
		SPVFuncImplFMul,
		SPVFuncImplFAdd,
		SPVFuncImplCubemapTo2DArrayFace,
		SPVFuncImplUnsafeArray, // Allow Metal to use the array<T> template to make arrays a value type
		SPVFuncImplInverse4x4,
		SPVFuncImplInverse3x3,
		SPVFuncImplInverse2x2,
		// It is very important that this come before *Swizzle and ChromaReconstruct*, to ensure it's
		// emitted before them.
		SPVFuncImplForwardArgs,
		// Likewise, this must come before *Swizzle.
		SPVFuncImplGetSwizzle,
		SPVFuncImplTextureSwizzle,
		SPVFuncImplGatherSwizzle,
		SPVFuncImplGatherCompareSwizzle,
		SPVFuncImplSubgroupBallot,
		SPVFuncImplSubgroupBallotBitExtract,
		SPVFuncImplSubgroupBallotFindLSB,
		SPVFuncImplSubgroupBallotFindMSB,
		SPVFuncImplSubgroupBallotBitCount,
		SPVFuncImplSubgroupAllEqual,
		SPVFuncImplReflectScalar,
		SPVFuncImplRefractScalar,
		SPVFuncImplFaceForwardScalar,
		SPVFuncImplChromaReconstructNearest2Plane,
		SPVFuncImplChromaReconstructNearest3Plane,
		SPVFuncImplChromaReconstructLinear422CositedEven2Plane,
		SPVFuncImplChromaReconstructLinear422CositedEven3Plane,
		SPVFuncImplChromaReconstructLinear422Midpoint2Plane,
		SPVFuncImplChromaReconstructLinear422Midpoint3Plane,
		SPVFuncImplChromaReconstructLinear420XCositedEvenYCositedEven2Plane,
		SPVFuncImplChromaReconstructLinear420XCositedEvenYCositedEven3Plane,
		SPVFuncImplChromaReconstructLinear420XMidpointYCositedEven2Plane,
		SPVFuncImplChromaReconstructLinear420XMidpointYCositedEven3Plane,
		SPVFuncImplChromaReconstructLinear420XCositedEvenYMidpoint2Plane,
		SPVFuncImplChromaReconstructLinear420XCositedEvenYMidpoint3Plane,
		SPVFuncImplChromaReconstructLinear420XMidpointYMidpoint2Plane,
		SPVFuncImplChromaReconstructLinear420XMidpointYMidpoint3Plane,
		SPVFuncImplExpandITUFullRange,
		SPVFuncImplExpandITUNarrowRange,
		SPVFuncImplConvertYCbCrBT709,
		SPVFuncImplConvertYCbCrBT601,
		SPVFuncImplConvertYCbCrBT2020,
		SPVFuncImplDynamicImageSampler,

		SPVFuncImplArrayCopyMultidimMax = 6
	};

	// If the underlying resource has been used for comparison then duplicate loads of that resource must be too
	// Use Metal's native frame-buffer fetch API for subpass inputs.
	void emit_texture_op(const Instruction &i, bool sparse) override;
	void emit_binary_unord_op(ezUInt32 result_type, ezUInt32 result_id, ezUInt32 op0, ezUInt32 op1, const char *op);
	void emit_instruction(const Instruction &instr) override;
	void emit_glsl_op(ezUInt32 result_type, ezUInt32 result_id, ezUInt32 op, const ezUInt32 *args,
	                  ezUInt32 count) override;
	void emit_spv_amd_shader_trinary_minmax_op(ezUInt32 result_type, ezUInt32 result_id, ezUInt32 op,
	                                           const ezUInt32 *args, ezUInt32 count) override;
	void emit_header() override;
	void emit_function_prototype(SPIRFunction &func, const Bitset &return_flags) override;
	void emit_sampled_image_op(ezUInt32 result_type, ezUInt32 result_id, ezUInt32 image_id, ezUInt32 samp_id) override;
	void emit_subgroup_op(const Instruction &i) override;
	std::string to_texture_op(const Instruction &i, bool sparse, bool *forward,
	                          SmallVector<ezUInt32> &inherited_expressions) override;
	void emit_fixup() override;
	std::string to_struct_member(const SPIRType &type, ezUInt32 member_type_id, ezUInt32 index,
	                             const std::string &qualifier = "");
	void emit_struct_member(const SPIRType &type, ezUInt32 member_type_id, ezUInt32 index,
	                        const std::string &qualifier = "", ezUInt32 base_offset = 0) override;
	void emit_struct_padding_target(const SPIRType &type) override;
	std::string type_to_glsl(const SPIRType &type, ezUInt32 id = 0) override;

	// Allow Metal to use the array<T> template to make arrays a value type
	std::string type_to_array_glsl(const SPIRType &type) override;

	// Threadgroup arrays can't have a wrapper type
	std::string variable_decl(const SPIRVariable &variable) override;

	// GCC workaround of lambdas calling protected functions (for older GCC versions)
	std::string variable_decl(const SPIRType &type, const std::string &name, ezUInt32 id = 0) override;

	std::string image_type_glsl(const SPIRType &type, ezUInt32 id = 0) override;
	std::string sampler_type(const SPIRType &type);
	std::string builtin_to_glsl(spv::BuiltIn builtin, spv::StorageClass storage) override;
	std::string to_func_call_arg(const SPIRFunction::Parameter &arg, ezUInt32 id) override;
	std::string to_name(ezUInt32 id, bool allow_alias = true) const override;
	std::string to_function_name(const TextureFunctionNameArguments &args) override;
	std::string to_function_args(const TextureFunctionArguments &args, bool *p_forward) override;
	std::string to_initializer_expression(const SPIRVariable &var) override;
	std::string to_zero_initialized_expression(ezUInt32 type_id) override;

	std::string unpack_expression_type(std::string expr_str, const SPIRType &type, ezUInt32 physical_type_id,
	                                   bool is_packed, bool row_major) override;

	// Returns true for BuiltInSampleMask because gl_SampleMask[] is an array in SPIR-V, but [[sample_mask]] is a scalar in Metal.
	bool builtin_translates_to_nonarray(spv::BuiltIn builtin) const override;

	std::string bitcast_glsl_op(const SPIRType &result_type, const SPIRType &argument_type) override;
	bool emit_complex_bitcast(ezUInt32 result_id, ezUInt32 id, ezUInt32 op0) override;
	bool skip_argument(ezUInt32 id) const override;
	std::string to_member_reference(ezUInt32 base, const SPIRType &type, ezUInt32 index, bool ptr_chain) override;
	std::string to_qualifiers_glsl(ezUInt32 id) override;
	void replace_illegal_names() override;
	void declare_undefined_values() override;
	void declare_constant_arrays();

	// Constant arrays of non-primitive types (i.e. matrices) won't link properly into Metal libraries
	void declare_complex_constant_arrays();

	bool is_patch_block(const SPIRType &type);
	bool is_non_native_row_major_matrix(ezUInt32 id) override;
	bool member_is_non_native_row_major_matrix(const SPIRType &type, ezUInt32 index) override;
	std::string convert_row_major_matrix(std::string exp_str, const SPIRType &exp_type, ezUInt32 physical_type_id,
	                                     bool is_packed) override;

	void preprocess_op_codes();
	void localize_global_variables();
	void extract_global_variables_from_functions();
	void mark_packable_structs();
	void mark_as_packable(SPIRType &type);

	std::unordered_map<ezUInt32, std::set<ezUInt32>> function_global_vars;
	void extract_global_variables_from_function(ezUInt32 func_id, std::set<ezUInt32> &added_arg_ids,
	                                            std::unordered_set<ezUInt32> &global_var_ids,
	                                            std::unordered_set<ezUInt32> &processed_func_ids);
	ezUInt32 add_interface_block(spv::StorageClass storage, bool patch = false);
	ezUInt32 add_interface_block_pointer(ezUInt32 ib_var_id, spv::StorageClass storage);

	struct InterfaceBlockMeta
	{
		struct LocationMeta
		{
			ezUInt32 num_components = 0;
			ezUInt32 ib_index = ~0u;
		};
		std::unordered_map<ezUInt32, LocationMeta> location_meta;
		bool strip_array = false;
	};

	void add_variable_to_interface_block(spv::StorageClass storage, const std::string &ib_var_ref, SPIRType &ib_type,
	                                     SPIRVariable &var, InterfaceBlockMeta &meta);
	void add_composite_variable_to_interface_block(spv::StorageClass storage, const std::string &ib_var_ref,
	                                               SPIRType &ib_type, SPIRVariable &var, InterfaceBlockMeta &meta);
	void add_plain_variable_to_interface_block(spv::StorageClass storage, const std::string &ib_var_ref,
	                                           SPIRType &ib_type, SPIRVariable &var, InterfaceBlockMeta &meta);
	void add_plain_member_variable_to_interface_block(spv::StorageClass storage, const std::string &ib_var_ref,
	                                                  SPIRType &ib_type, SPIRVariable &var, ezUInt32 index,
	                                                  InterfaceBlockMeta &meta);
	void add_composite_member_variable_to_interface_block(spv::StorageClass storage, const std::string &ib_var_ref,
	                                                      SPIRType &ib_type, SPIRVariable &var, ezUInt32 index,
	                                                      InterfaceBlockMeta &meta);
	ezUInt32 get_accumulated_member_location(const SPIRVariable &var, ezUInt32 mbr_idx, bool strip_array);
	void add_tess_level_input_to_interface_block(const std::string &ib_var_ref, SPIRType &ib_type, SPIRVariable &var);

	void fix_up_interface_member_indices(spv::StorageClass storage, ezUInt32 ib_type_id);

	void mark_location_as_used_by_shader(ezUInt32 location, spv::StorageClass storage);
	ezUInt32 ensure_correct_builtin_type(ezUInt32 type_id, spv::BuiltIn builtin);
	ezUInt32 ensure_correct_input_type(ezUInt32 type_id, ezUInt32 location, ezUInt32 num_components = 0);

	void emit_custom_templates();
	void emit_custom_functions();
	void emit_resources();
	void emit_specialization_constants_and_structs();
	void emit_interface_block(ezUInt32 ib_var_id);
	bool maybe_emit_array_assignment(ezUInt32 id_lhs, ezUInt32 id_rhs);

	void fix_up_shader_inputs_outputs();

	std::string func_type_decl(SPIRType &type);
	std::string entry_point_args_classic(bool append_comma);
	std::string entry_point_args_argument_buffer(bool append_comma);
	std::string entry_point_arg_stage_in();
	void entry_point_args_builtin(std::string &args);
	void entry_point_args_discrete_descriptors(std::string &args);
	std::string to_qualified_member_name(const SPIRType &type, ezUInt32 index);
	std::string ensure_valid_name(std::string name, std::string pfx);
	std::string to_sampler_expression(ezUInt32 id);
	std::string to_swizzle_expression(ezUInt32 id);
	std::string to_buffer_size_expression(ezUInt32 id);
	std::string builtin_qualifier(spv::BuiltIn builtin);
	std::string builtin_type_decl(spv::BuiltIn builtin, ezUInt32 id = 0);
	std::string built_in_func_arg(spv::BuiltIn builtin, bool prefix_comma);
	std::string member_attribute_qualifier(const SPIRType &type, ezUInt32 index);
	std::string argument_decl(const SPIRFunction::Parameter &arg);
	std::string round_fp_tex_coords(std::string tex_coords, bool coord_is_fp);
	ezUInt32 get_metal_resource_index(SPIRVariable &var, SPIRType::BaseType basetype, ezUInt32 plane = 0);
	ezUInt32 get_ordered_member_location(ezUInt32 type_id, ezUInt32 index, ezUInt32 *comp = nullptr);

	// MSL packing rules. These compute the effective packing rules as observed by the MSL compiler in the MSL output.
	// These values can change depending on various extended decorations which control packing rules.
	// We need to make these rules match up with SPIR-V declared rules.
	ezUInt32 get_declared_type_size_msl(const SPIRType &type, bool packed, bool row_major) const;
	ezUInt32 get_declared_type_array_stride_msl(const SPIRType &type, bool packed, bool row_major) const;
	ezUInt32 get_declared_type_matrix_stride_msl(const SPIRType &type, bool packed, bool row_major) const;
	ezUInt32 get_declared_type_alignment_msl(const SPIRType &type, bool packed, bool row_major) const;

	ezUInt32 get_declared_struct_member_size_msl(const SPIRType &struct_type, ezUInt32 index) const;
	ezUInt32 get_declared_struct_member_array_stride_msl(const SPIRType &struct_type, ezUInt32 index) const;
	ezUInt32 get_declared_struct_member_matrix_stride_msl(const SPIRType &struct_type, ezUInt32 index) const;
	ezUInt32 get_declared_struct_member_alignment_msl(const SPIRType &struct_type, ezUInt32 index) const;

	const SPIRType &get_physical_member_type(const SPIRType &struct_type, ezUInt32 index) const;

	ezUInt32 get_declared_struct_size_msl(const SPIRType &struct_type, bool ignore_alignment = false,
	                                      bool ignore_padding = false) const;

	std::string to_component_argument(ezUInt32 id);
	void align_struct(SPIRType &ib_type, std::unordered_set<ezUInt32> &aligned_structs);
	void mark_scalar_layout_structs(const SPIRType &ib_type);
	void mark_struct_members_packed(const SPIRType &type);
	void ensure_member_packing_rules_msl(SPIRType &ib_type, ezUInt32 index);
	bool validate_member_packing_rules_msl(const SPIRType &type, ezUInt32 index) const;
	std::string get_argument_address_space(const SPIRVariable &argument);
	std::string get_type_address_space(const SPIRType &type, ezUInt32 id, bool argument = false);
	const char *to_restrict(ezUInt32 id, bool space = true);
	SPIRType &get_stage_in_struct_type();
	SPIRType &get_stage_out_struct_type();
	SPIRType &get_patch_stage_in_struct_type();
	SPIRType &get_patch_stage_out_struct_type();
	std::string get_tess_factor_struct_name();
	void emit_atomic_func_op(ezUInt32 result_type, ezUInt32 result_id, const char *op, ezUInt32 mem_order_1,
	                         ezUInt32 mem_order_2, bool has_mem_order_2, ezUInt32 op0, ezUInt32 op1 = 0,
	                         bool op1_is_pointer = false, bool op1_is_literal = false, ezUInt32 op2 = 0);
	const char *get_memory_order(ezUInt32 spv_mem_sem);
	void add_pragma_line(const std::string &line);
	void add_typedef_line(const std::string &line);
	void emit_barrier(ezUInt32 id_exe_scope, ezUInt32 id_mem_scope, ezUInt32 id_mem_sem);
	void emit_array_copy(const std::string &lhs, ezUInt32 rhs_id, spv::StorageClass lhs_storage,
	                     spv::StorageClass rhs_storage) override;
	void build_implicit_builtins();
	ezUInt32 build_constant_uint_array_pointer();
	void emit_entry_point_declarations() override;
	ezUInt32 builtin_frag_coord_id = 0;
	ezUInt32 builtin_sample_id_id = 0;
	ezUInt32 builtin_vertex_idx_id = 0;
	ezUInt32 builtin_base_vertex_id = 0;
	ezUInt32 builtin_instance_idx_id = 0;
	ezUInt32 builtin_base_instance_id = 0;
	ezUInt32 builtin_view_idx_id = 0;
	ezUInt32 builtin_layer_id = 0;
	ezUInt32 builtin_invocation_id_id = 0;
	ezUInt32 builtin_primitive_id_id = 0;
	ezUInt32 builtin_subgroup_invocation_id_id = 0;
	ezUInt32 builtin_subgroup_size_id = 0;
	ezUInt32 builtin_dispatch_base_id = 0;
	ezUInt32 swizzle_buffer_id = 0;
	ezUInt32 buffer_size_buffer_id = 0;
	ezUInt32 view_mask_buffer_id = 0;
	ezUInt32 dynamic_offsets_buffer_id = 0;

	void bitcast_to_builtin_store(ezUInt32 target_id, std::string &expr, const SPIRType &expr_type) override;
	void bitcast_from_builtin_load(ezUInt32 source_id, std::string &expr, const SPIRType &expr_type) override;
	void emit_store_statement(ezUInt32 lhs_expression, ezUInt32 rhs_expression) override;

	void analyze_sampled_image_usage();

	bool emit_tessellation_access_chain(const ezUInt32 *ops, ezUInt32 length);
	bool emit_tessellation_io_load(ezUInt32 result_type, ezUInt32 id, ezUInt32 ptr);
	bool is_out_of_bounds_tessellation_level(ezUInt32 id_lhs);

	void ensure_builtin(spv::StorageClass storage, spv::BuiltIn builtin);

	void mark_implicit_builtin(spv::StorageClass storage, spv::BuiltIn builtin, ezUInt32 id);

	std::string convert_to_f32(const std::string &expr, ezUInt32 components);

	Options msl_options;
	std::set<SPVFuncImpl> spv_function_implementations;
	std::unordered_map<ezUInt32, MSLShaderInput> inputs_by_location;
	std::unordered_map<ezUInt32, MSLShaderInput> inputs_by_builtin;
	std::unordered_set<ezUInt32> inputs_in_use;
	std::unordered_map<ezUInt32, ezUInt32> fragment_output_components;
	std::set<std::string> pragma_lines;
	std::set<std::string> typedef_lines;
	SmallVector<ezUInt32> vars_needing_early_declaration;

	std::unordered_map<StageSetBinding, std::pair<MSLResourceBinding, bool>, InternalHasher> resource_bindings;

	ezUInt32 next_metal_resource_index_buffer = 0;
	ezUInt32 next_metal_resource_index_texture = 0;
	ezUInt32 next_metal_resource_index_sampler = 0;
	// Intentionally uninitialized, works around MSVC 2013 bug.
	ezUInt32 next_metal_resource_ids[kMaxArgumentBuffers];

	VariableID stage_in_var_id = 0;
	VariableID stage_out_var_id = 0;
	VariableID patch_stage_in_var_id = 0;
	VariableID patch_stage_out_var_id = 0;
	VariableID stage_in_ptr_var_id = 0;
	VariableID stage_out_ptr_var_id = 0;

	// Handle HLSL-style 0-based vertex/instance index.
	enum class TriState
	{
		Neutral,
		No,
		Yes
	};
	TriState needs_base_vertex_arg = TriState::Neutral;
	TriState needs_base_instance_arg = TriState::Neutral;

	bool has_sampled_images = false;
	bool builtin_declaration = false; // Handle HLSL-style 0-based vertex/instance index.

	bool is_using_builtin_array = false; // Force the use of C style array declaration.
	bool using_builtin_array() const;

	bool is_rasterization_disabled = false;
	bool capture_output_to_buffer = false;
	bool needs_swizzle_buffer_def = false;
	bool used_swizzle_buffer = false;
	bool added_builtin_tess_level = false;
	bool needs_subgroup_invocation_id = false;
	std::string qual_pos_var_name;
	std::string stage_in_var_name = "in";
	std::string stage_out_var_name = "out";
	std::string patch_stage_in_var_name = "patchIn";
	std::string patch_stage_out_var_name = "patchOut";
	std::string sampler_name_suffix = "Smplr";
	std::string swizzle_name_suffix = "Swzl";
	std::string buffer_size_name_suffix = "BufferSize";
	std::string plane_name_suffix = "Plane";
	std::string input_wg_var_name = "gl_in";
	std::string output_buffer_var_name = "spvOut";
	std::string patch_output_buffer_var_name = "spvPatchOut";
	std::string tess_factor_buffer_var_name = "spvTessLevel";
	spv::Op previous_instruction_opcode = spv::OpNop;

	// Must be ordered since declaration is in a specific order.
	std::map<ezUInt32, MSLConstexprSampler> constexpr_samplers_by_id;
	std::unordered_map<SetBindingPair, MSLConstexprSampler, InternalHasher> constexpr_samplers_by_binding;
	const MSLConstexprSampler *find_constexpr_sampler(ezUInt32 id) const;

	std::unordered_set<ezUInt32> buffers_requiring_array_length;
	SmallVector<ezUInt32> buffer_arrays;
	std::unordered_set<ezUInt32> atomic_image_vars; // Emulate texture2D atomic operations

	// Must be ordered since array is in a specific order.
	std::map<SetBindingPair, std::pair<ezUInt32, ezUInt32>> buffers_requiring_dynamic_offset;

	SmallVector<ezUInt32> disabled_frag_outputs;

	std::unordered_set<SetBindingPair, InternalHasher> inline_uniform_blocks;

	ezUInt32 argument_buffer_ids[kMaxArgumentBuffers];
	ezUInt32 argument_buffer_discrete_mask = 0;
	ezUInt32 argument_buffer_device_storage_mask = 0;

	void analyze_argument_buffers();
	bool descriptor_set_is_argument_buffer(ezUInt32 desc_set) const;

	ezUInt32 get_target_components_for_fragment_location(ezUInt32 location) const;
	ezUInt32 build_extended_vector_type(ezUInt32 type_id, ezUInt32 components, SPIRType::BaseType basetype = SPIRType::Unknown);

	bool suppress_missing_prototypes = false;

	void add_spv_func_and_recompile(SPVFuncImpl spv_func);

	void activate_argument_buffer_resources();

	bool type_is_msl_framebuffer_fetch(const SPIRType &type) const;

	// OpcodeHandler that handles several MSL preprocessing operations.
	struct OpCodePreprocessor : OpcodeHandler
	{
		OpCodePreprocessor(CompilerMSL &compiler_)
		    : compiler(compiler_)
		{
		}

		bool handle(spv::Op opcode, const ezUInt32 *args, ezUInt32 length) override;
		CompilerMSL::SPVFuncImpl get_spv_func_impl(spv::Op opcode, const ezUInt32 *args);
		void check_resource_write(ezUInt32 var_id);

		CompilerMSL &compiler;
		std::unordered_map<ezUInt32, ezUInt32> result_types;
		std::unordered_map<ezUInt32, ezUInt32> image_pointers; // Emulate texture2D atomic operations
		bool suppress_missing_prototypes = false;
		bool uses_atomics = false;
		bool uses_resource_write = false;
		bool needs_subgroup_invocation_id = false;
	};

	// OpcodeHandler that scans for uses of sampled images
	struct SampledImageScanner : OpcodeHandler
	{
		SampledImageScanner(CompilerMSL &compiler_)
		    : compiler(compiler_)
		{
		}

		bool handle(spv::Op opcode, const ezUInt32 *args, ezUInt32) override;

		CompilerMSL &compiler;
	};

	// Sorts the members of a SPIRType and associated Meta info based on a settable sorting
	// aspect, which defines which aspect of the struct members will be used to sort them.
	// Regardless of the sorting aspect, built-in members always appear at the end of the struct.
	struct MemberSorter
	{
		enum SortAspect
		{
			Location,
			LocationReverse,
			Offset,
			OffsetThenLocationReverse,
			Alphabetical
		};

		void sort();
		bool operator()(ezUInt32 mbr_idx1, ezUInt32 mbr_idx2);
		MemberSorter(SPIRType &t, Meta &m, SortAspect sa);

		SPIRType &type;
		Meta &meta;
		SortAspect sort_aspect;
	};
};
} // namespace SPIRV_CROSS_NAMESPACE

#endif
