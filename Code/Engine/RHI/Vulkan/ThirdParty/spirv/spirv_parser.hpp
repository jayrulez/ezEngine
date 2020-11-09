/*
 * Copyright 2018-2020 Arm Limited
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

#ifndef SPIRV_CROSS_PARSER_HPP
#define SPIRV_CROSS_PARSER_HPP

#include <RHI/Vulkan/ThirdParty/spirv/spirv_cross_parsed_ir.hpp>
#include <stdint.h>

namespace SPIRV_CROSS_NAMESPACE
{
class Parser
{
public:
	Parser(const ezUInt32 *spirv_data, size_t word_count);
	Parser(std::vector<ezUInt32> spirv);

	void parse();

	ParsedIR &get_parsed_ir()
	{
		return ir;
	}

private:
	ParsedIR ir;
	SPIRFunction *current_function = nullptr;
	SPIRBlock *current_block = nullptr;

	void parse(const Instruction &instr);
	const ezUInt32 *stream(const Instruction &instr) const;

	template <typename T, typename... P>
	T &set(ezUInt32 id, P &&... args)
	{
		ir.add_typed_id(static_cast<Types>(T::type), id);
		auto &var = variant_set<T>(ir.ids[id], std::forward<P>(args)...);
		var.self = id;
		return var;
	}

	template <typename T>
	T &get(ezUInt32 id)
	{
		return variant_get<T>(ir.ids[id]);
	}

	template <typename T>
	T *maybe_get(ezUInt32 id)
	{
		if (ir.ids[id].get_type() == static_cast<Types>(T::type))
			return &get<T>(id);
		else
			return nullptr;
	}

	template <typename T>
	const T &get(ezUInt32 id) const
	{
		return variant_get<T>(ir.ids[id]);
	}

	template <typename T>
	const T *maybe_get(ezUInt32 id) const
	{
		if (ir.ids[id].get_type() == T::type)
			return &get<T>(id);
		else
			return nullptr;
	}

	// This must be an ordered data structure so we always pick the same type aliases.
	SmallVector<ezUInt32> global_struct_cache;
	SmallVector<std::pair<ezUInt32, ezUInt32>> forward_pointer_fixups;

	bool types_are_logically_equivalent(const SPIRType &a, const SPIRType &b) const;
	bool variable_storage_is_aliased(const SPIRVariable &v) const;
};
} // namespace SPIRV_CROSS_NAMESPACE

#endif
