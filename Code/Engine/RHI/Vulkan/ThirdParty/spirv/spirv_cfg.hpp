/*
 * Copyright 2016-2020 Arm Limited
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

#ifndef SPIRV_CROSS_CFG_HPP
#define SPIRV_CROSS_CFG_HPP

#include <RHI/Vulkan/ThirdParty/spirv/spirv_common.hpp>
#include <assert.h>

namespace SPIRV_CROSS_NAMESPACE
{
class Compiler;
class CFG
{
public:
	CFG(Compiler &compiler, const SPIRFunction &function);

	Compiler &get_compiler()
	{
		return compiler;
	}

	const Compiler &get_compiler() const
	{
		return compiler;
	}

	const SPIRFunction &get_function() const
	{
		return func;
	}

	ezUInt32 get_immediate_dominator(ezUInt32 block) const
	{
		auto itr = immediate_dominators.find(block);
		if (itr != std::end(immediate_dominators))
			return itr->second;
		else
			return 0;
	}

	ezUInt32 get_visit_order(ezUInt32 block) const
	{
		auto itr = visit_order.find(block);
		assert(itr != std::end(visit_order));
		int v = itr->second.get();
		assert(v > 0);
		return ezUInt32(v);
	}

	ezUInt32 find_common_dominator(ezUInt32 a, ezUInt32 b) const;

	const SmallVector<ezUInt32> &get_preceding_edges(ezUInt32 block) const
	{
		auto itr = preceding_edges.find(block);
		if (itr != std::end(preceding_edges))
			return itr->second;
		else
			return empty_vector;
	}

	const SmallVector<ezUInt32> &get_succeeding_edges(ezUInt32 block) const
	{
		auto itr = succeeding_edges.find(block);
		if (itr != std::end(succeeding_edges))
			return itr->second;
		else
			return empty_vector;
	}

	template <typename Op>
	void walk_from(std::unordered_set<ezUInt32> &seen_blocks, ezUInt32 block, const Op &op) const
	{
		if (seen_blocks.count(block))
			return;
		seen_blocks.insert(block);

		if (op(block))
		{
			for (auto b : get_succeeding_edges(block))
				walk_from(seen_blocks, b, op);
		}
	}

	ezUInt32 find_loop_dominator(ezUInt32 block) const;

	bool node_terminates_control_flow_in_sub_graph(BlockID from, BlockID to) const;

private:
	struct VisitOrder
	{
		int &get()
		{
			return v;
		}

		const int &get() const
		{
			return v;
		}

		int v = -1;
	};

	Compiler &compiler;
	const SPIRFunction &func;
	std::unordered_map<ezUInt32, SmallVector<ezUInt32>> preceding_edges;
	std::unordered_map<ezUInt32, SmallVector<ezUInt32>> succeeding_edges;
	std::unordered_map<ezUInt32, ezUInt32> immediate_dominators;
	std::unordered_map<ezUInt32, VisitOrder> visit_order;
	SmallVector<ezUInt32> post_order;
	SmallVector<ezUInt32> empty_vector;

	void add_branch(ezUInt32 from, ezUInt32 to);
	void build_post_order_visit_order();
	void build_immediate_dominators();
	bool post_order_visit(ezUInt32 block);
	ezUInt32 visit_count = 0;

	bool is_back_edge(ezUInt32 to) const;
	bool has_visited_forward_edge(ezUInt32 to) const;
};

class DominatorBuilder
{
public:
	DominatorBuilder(const CFG &cfg);

	void add_block(ezUInt32 block);
	ezUInt32 get_dominator() const
	{
		return dominator;
	}

	void lift_continue_block_dominator();

private:
	const CFG &cfg;
	ezUInt32 dominator = 0;
};
} // namespace SPIRV_CROSS_NAMESPACE

#endif
