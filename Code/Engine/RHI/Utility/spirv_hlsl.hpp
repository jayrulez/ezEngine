/*
 * Copyright 2016-2020 Robert Konrad
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

#ifndef SPIRV_HLSL_HPP
#define SPIRV_HLSL_HPP

#include <RHI/Utility/spirv_glsl.hpp>
#include <utility>

namespace SPIRV_CROSS_NAMESPACE
{
// Interface which remaps vertex inputs to a fixed semantic name to make linking easier.
struct HLSLVertexAttributeRemap
{
	ezUInt32 location;
	std::string semantic;
};
// Specifying a root constant (d3d12) or push constant range (vulkan).
//
// `start` and `end` denotes the range of the root constant in bytes.
// Both values need to be multiple of 4.
struct RootConstants
{
	ezUInt32 start;
	ezUInt32 end;

	ezUInt32 binding;
	ezUInt32 space;
};

// For finer control, decorations may be removed from specific resources instead with unset_decoration().
enum HLSLBindingFlagBits
{
	HLSL_BINDING_AUTO_NONE_BIT = 0,

	// Push constant (root constant) resources will be declared as CBVs (b-space) without a register() declaration.
	// A register will be automatically assigned by the D3D compiler, but must therefore be reflected in D3D-land.
	// Push constants do not normally have a DecorationBinding set, but if they do, this can be used to ignore it.
	HLSL_BINDING_AUTO_PUSH_CONSTANT_BIT = 1 << 0,

	// cbuffer resources will be declared as CBVs (b-space) without a register() declaration.
	// A register will be automatically assigned, but must be reflected in D3D-land.
	HLSL_BINDING_AUTO_CBV_BIT = 1 << 1,

	// All SRVs (t-space) will be declared without a register() declaration.
	HLSL_BINDING_AUTO_SRV_BIT = 1 << 2,

	// All UAVs (u-space) will be declared without a register() declaration.
	HLSL_BINDING_AUTO_UAV_BIT = 1 << 3,

	// All samplers (s-space) will be declared without a register() declaration.
	HLSL_BINDING_AUTO_SAMPLER_BIT = 1 << 4,

	// No resources will be declared with register().
	HLSL_BINDING_AUTO_ALL = 0x7fffffff
};
using HLSLBindingFlags = ezUInt32;

// By matching stage, desc_set and binding for a SPIR-V resource,
// register bindings are set based on whether the HLSL resource is a
// CBV, UAV, SRV or Sampler. A single binding in SPIR-V might contain multiple
// resource types, e.g. COMBINED_IMAGE_SAMPLER, and SRV/Sampler bindings will be used respectively.
// On SM 5.0 and lower, register_space is ignored.
//
// To remap a push constant block which does not have any desc_set/binding associated with it,
// use ResourceBindingPushConstant{DescriptorSet,Binding} as values for desc_set/binding.
// For deeper control of push constants, set_root_constant_layouts() can be used instead.
struct HLSLResourceBinding
{
	spv::ExecutionModel stage = spv::ExecutionModelMax;
	ezUInt32 desc_set = 0;
	ezUInt32 binding = 0;

	struct Binding
	{
		ezUInt32 register_space = 0;
		ezUInt32 register_binding = 0;
	} cbv, uav, srv, sampler;
};

class CompilerHLSL : public CompilerGLSL
{
public:
	struct Options
	{
		ezUInt32 shader_model = 30; // TODO: map ps_4_0_level_9_0,... somehow

		// Allows the PointSize builtin, and ignores it, as PointSize is not supported in HLSL.
		bool point_size_compat = false;

		// Allows the PointCoord builtin, returns float2(0.5, 0.5), as PointCoord is not supported in HLSL.
		bool point_coord_compat = false;

		// If true, the backend will assume that VertexIndex and InstanceIndex will need to apply
		// a base offset, and you will need to fill in a cbuffer with offsets.
		// Set to false if you know you will never use base instance or base vertex
		// functionality as it might remove an internal cbuffer.
		bool support_nonzero_base_vertex_base_instance = false;

		// Forces a storage buffer to always be declared as UAV, even if the readonly decoration is used.
		// By default, a readonly storage buffer will be declared as ByteAddressBuffer (SRV) instead.
		// Alternatively, use set_hlsl_force_storage_buffer_as_uav to specify individually.
		bool force_storage_buffer_as_uav = false;

		// Forces any storage image type marked as NonWritable to be considered an SRV instead.
		// For this to work with function call parameters, NonWritable must be considered to be part of the type system
		// so that NonWritable image arguments are also translated to Texture rather than RWTexture.
		bool nonwritable_uav_texture_as_srv = false;

		// Enables native 16-bit types. Needs SM 6.2.
		// Uses half/int16_t/ezUInt16 instead of min16* types.
		// Also adds support for 16-bit load-store from (RW)ByteAddressBuffer.
		bool enable_16bit_types = false;
	};

	explicit CompilerHLSL(std::vector<ezUInt32> spirv_)
	    : CompilerGLSL(std::move(spirv_))
	{
	}

	CompilerHLSL(const ezUInt32 *ir_, size_t size)
	    : CompilerGLSL(ir_, size)
	{
	}

	explicit CompilerHLSL(const ParsedIR &ir_)
	    : CompilerGLSL(ir_)
	{
	}

	explicit CompilerHLSL(ParsedIR &&ir_)
	    : CompilerGLSL(std::move(ir_))
	{
	}

	const Options &get_hlsl_options() const
	{
		return hlsl_options;
	}

	void set_hlsl_options(const Options &opts)
	{
		hlsl_options = opts;
	}

	// Optionally specify a custom root constant layout.
	//
	// Push constants ranges will be split up according to the
	// layout specified.
	void set_root_constant_layouts(std::vector<RootConstants> layout);

	// Compiles and remaps vertex attributes at specific locations to a fixed semantic.
	// The default is TEXCOORD# where # denotes location.
	// Matrices are unrolled to vectors with notation ${SEMANTIC}_#, where # denotes row.
	// $SEMANTIC is either TEXCOORD# or a semantic name specified here.
	void add_vertex_attribute_remap(const HLSLVertexAttributeRemap &vertex_attributes);
	std::string compile() override;

	// This is a special HLSL workaround for the NumWorkGroups builtin.
	// This does not exist in HLSL, so the calling application must create a dummy cbuffer in
	// which the application will store this builtin.
	// The cbuffer layout will be:
	// cbuffer SPIRV_Cross_NumWorkgroups : register(b#, space#) { uint3 SPIRV_Cross_NumWorkgroups_count; };
	// This must be called before compile().
	// The function returns 0 if NumWorkGroups builtin is not statically used in the shader from the current entry point.
	// If non-zero, this returns the variable ID of a cbuffer which corresponds to
	// the cbuffer declared above. By default, no binding or descriptor set decoration is set,
	// so the calling application should declare explicit bindings on this ID before calling compile().
	VariableID remap_num_workgroups_builtin();

	// Controls how resource bindings are declared in the output HLSL.
	void set_resource_binding_flags(HLSLBindingFlags flags);

	// resource is a resource binding to indicate the HLSL CBV, SRV, UAV or sampler binding
	// to use for a particular SPIR-V description set
	// and binding. If resource bindings are provided,
	// is_hlsl_resource_binding_used() will return true after calling ::compile() if
	// the set/binding combination was used by the HLSL code.
	void add_hlsl_resource_binding(const HLSLResourceBinding &resource);
	bool is_hlsl_resource_binding_used(spv::ExecutionModel model, ezUInt32 set, ezUInt32 binding) const;

	// Controls which storage buffer bindings will be forced to be declared as UAVs.
	void set_hlsl_force_storage_buffer_as_uav(ezUInt32 desc_set, ezUInt32 binding);

private:
	std::string type_to_glsl(const SPIRType &type, ezUInt32 id = 0) override;
	std::string image_type_hlsl(const SPIRType &type, ezUInt32 id);
	std::string image_type_hlsl_modern(const SPIRType &type, ezUInt32 id);
	std::string image_type_hlsl_legacy(const SPIRType &type, ezUInt32 id);
	void emit_function_prototype(SPIRFunction &func, const Bitset &return_flags) override;
	void emit_hlsl_entry_point();
	void emit_header() override;
	void emit_resources();
	void declare_undefined_values() override;
	void emit_interface_block_globally(const SPIRVariable &type);
	void emit_interface_block_in_struct(const SPIRVariable &type, std::unordered_set<ezUInt32> &active_locations);
	void emit_builtin_inputs_in_struct();
	void emit_builtin_outputs_in_struct();
	void emit_texture_op(const Instruction &i, bool sparse) override;
	void emit_instruction(const Instruction &instruction) override;
	void emit_glsl_op(ezUInt32 result_type, ezUInt32 result_id, ezUInt32 op, const ezUInt32 *args,
	                  ezUInt32 count) override;
	void emit_buffer_block(const SPIRVariable &type) override;
	void emit_push_constant_block(const SPIRVariable &var) override;
	void emit_uniform(const SPIRVariable &var) override;
	void emit_modern_uniform(const SPIRVariable &var);
	void emit_legacy_uniform(const SPIRVariable &var);
	void emit_specialization_constants_and_structs();
	void emit_composite_constants();
	void emit_fixup() override;
	std::string builtin_to_glsl(spv::BuiltIn builtin, spv::StorageClass storage) override;
	std::string layout_for_member(const SPIRType &type, ezUInt32 index) override;
	std::string to_interpolation_qualifiers(const Bitset &flags) override;
	std::string bitcast_glsl_op(const SPIRType &result_type, const SPIRType &argument_type) override;
	bool emit_complex_bitcast(ezUInt32 result_type, ezUInt32 id, ezUInt32 op0) override;
	std::string to_func_call_arg(const SPIRFunction::Parameter &arg, ezUInt32 id) override;
	std::string to_sampler_expression(ezUInt32 id);
	std::string to_resource_binding(const SPIRVariable &var);
	std::string to_resource_binding_sampler(const SPIRVariable &var);
	std::string to_resource_register(HLSLBindingFlagBits flag, char space, ezUInt32 binding, ezUInt32 set);
	void emit_sampled_image_op(ezUInt32 result_type, ezUInt32 result_id, ezUInt32 image_id, ezUInt32 samp_id) override;
	void emit_access_chain(const Instruction &instruction);
	void emit_load(const Instruction &instruction);
	void read_access_chain(std::string *expr, const std::string &lhs, const SPIRAccessChain &chain);
	void read_access_chain_struct(const std::string &lhs, const SPIRAccessChain &chain);
	void read_access_chain_array(const std::string &lhs, const SPIRAccessChain &chain);
	void write_access_chain(const SPIRAccessChain &chain, ezUInt32 value, const SmallVector<ezUInt32> &composite_chain);
	void write_access_chain_struct(const SPIRAccessChain &chain, ezUInt32 value,
	                               const SmallVector<ezUInt32> &composite_chain);
	void write_access_chain_array(const SPIRAccessChain &chain, ezUInt32 value,
	                              const SmallVector<ezUInt32> &composite_chain);
	std::string write_access_chain_value(ezUInt32 value, const SmallVector<ezUInt32> &composite_chain, bool enclose);
	void emit_store(const Instruction &instruction);
	void emit_atomic(const ezUInt32 *ops, ezUInt32 length, spv::Op op);
	void emit_subgroup_op(const Instruction &i) override;
	void emit_block_hints(const SPIRBlock &block) override;

	void emit_struct_member(const SPIRType &type, ezUInt32 member_type_id, ezUInt32 index, const std::string &qualifier,
	                        ezUInt32 base_offset = 0) override;

	const char *to_storage_qualifiers_glsl(const SPIRVariable &var) override;
	void replace_illegal_names() override;

	bool is_hlsl_force_storage_buffer_as_uav(ID id) const;

	Options hlsl_options;

	// TODO: Refactor this to be more similar to MSL, maybe have some common system in place?
	bool requires_op_fmod = false;
	bool requires_fp16_packing = false;
	bool requires_uint2_packing = false;
	bool requires_explicit_fp16_packing = false;
	bool requires_unorm8_packing = false;
	bool requires_snorm8_packing = false;
	bool requires_unorm16_packing = false;
	bool requires_snorm16_packing = false;
	bool requires_bitfield_insert = false;
	bool requires_bitfield_extract = false;
	bool requires_inverse_2x2 = false;
	bool requires_inverse_3x3 = false;
	bool requires_inverse_4x4 = false;
	bool requires_scalar_reflect = false;
	bool requires_scalar_refract = false;
	bool requires_scalar_faceforward = false;

	struct TextureSizeVariants
	{
		// MSVC 2013 workaround.
		TextureSizeVariants()
		{
			srv = 0;
			for (auto &unorm : uav)
				for (auto &u : unorm)
					u = 0;
		}
		uint64_t srv;
		uint64_t uav[3][4];
	} required_texture_size_variants;

	void require_texture_query_variant(ezUInt32 var_id);
	void emit_texture_size_variants(uint64_t variant_mask, const char *vecsize_qualifier, bool uav, const char *type_qualifier);

	enum TextureQueryVariantDim
	{
		Query1D = 0,
		Query1DArray,
		Query2D,
		Query2DArray,
		Query3D,
		QueryBuffer,
		QueryCube,
		QueryCubeArray,
		Query2DMS,
		Query2DMSArray,
		QueryDimCount
	};

	enum TextureQueryVariantType
	{
		QueryTypeFloat = 0,
		QueryTypeInt = 16,
		QueryTypeUInt = 32,
		QueryTypeCount = 3
	};

	enum BitcastType
	{
		TypeNormal,
		TypePackUint2x32,
		TypeUnpackUint64
	};

	BitcastType get_bitcast_type(ezUInt32 result_type, ezUInt32 op0);

	void emit_builtin_variables();
	bool require_output = false;
	bool require_input = false;
	SmallVector<HLSLVertexAttributeRemap> remap_vertex_attributes;

	ezUInt32 type_to_consumed_locations(const SPIRType &type) const;

	void emit_io_block(const SPIRVariable &var);
	std::string to_semantic(ezUInt32 location, spv::ExecutionModel em, spv::StorageClass sc);

	ezUInt32 num_workgroups_builtin = 0;
	HLSLBindingFlags resource_binding_flags = 0;

	// Custom root constant layout, which should be emitted
	// when translating push constant ranges.
	std::vector<RootConstants> root_constants_layout;

	void validate_shader_model();

	std::string get_unique_identifier();
	ezUInt32 unique_identifier_count = 0;

	std::unordered_map<StageSetBinding, std::pair<HLSLResourceBinding, bool>, InternalHasher> resource_bindings;
	void remap_hlsl_resource_binding(HLSLBindingFlagBits type, ezUInt32 &desc_set, ezUInt32 &binding);

	std::unordered_set<SetBindingPair, InternalHasher> force_uav_buffer_bindings;
};
} // namespace SPIRV_CROSS_NAMESPACE

#endif
