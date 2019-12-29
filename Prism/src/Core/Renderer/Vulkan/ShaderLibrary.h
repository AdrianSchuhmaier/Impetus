#pragma once

#include <unordered_map>

#include "../BufferUtil.h"
#include "Pipeline.h"

namespace Prism {

	class ShaderLibrary
	{
	public:
		static bool Load(const std::string& file, const VertexInputDescription& inputDescription);
		static void Remove(const std::string& file);

		static void CleanUp() { s_Pipelines.clear(); };

		static Vulkan::Pipeline* PipelineOf(const std::string& file)
		{
			auto s = s_Pipelines.size();
			auto it = s_Pipelines.find(file);
			if (it != s_Pipelines.end())
				return it->second.get();

			PR_CORE_ASSERT(false, "No Pipeline found for shader '{0}'", file);
			return nullptr;
		}

	private:
		static std::unordered_map<std::string, std::unique_ptr<Vulkan::Pipeline>> s_Pipelines;
	};

}