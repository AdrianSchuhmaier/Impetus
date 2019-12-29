#include "pch.h"
#include "ShaderLibrary.h"

#include "Pipeline.h"
#include "RenderPass.h"

#include <shaderc/shaderc.hpp>
#include <fstream>
#include <sstream>
#include <streambuf>

namespace Prism {
	using namespace Vulkan;

	// forward declarations
	std::optional<Shader::Code> readFile(const std::string& file);
	std::optional<Shader::SpirV> compile(const Shader::Code& code, const char* name = "shader");
	Vulkan::VertexBufferDescriptor generateVulkanDescriptor(const VertexInputDescription& genericDescriptor);
	vk::Format generateVulkanDataType(const BufferDataType& dataType);

	std::unordered_map<std::string, std::unique_ptr<Pipeline>> ShaderLibrary::s_Pipelines;

	bool ShaderLibrary::Load(const std::string& file, const VertexInputDescription& inputDescription)
	{
		auto it = s_Pipelines.find(file);
		if (it != s_Pipelines.end())
		{
			PR_CORE_WARN("Shader already exists");
			// TODO: reload
			return false;
		}
		else
		{
			// get shader name
			auto lastSlash = file.find_last_of("/");
			lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
			auto name = file.substr(lastSlash, file.size() - lastSlash);

			// read + compile
			auto code = readFile(file);
			PR_CORE_ASSERT(code.has_value(), "No shader code could be read to compile '{0}'", name);

			auto spv = compile(code.value(), name.c_str());
			PR_CORE_ASSERT(spv.has_value(), "Unable to compile shader '{0}'", name);

			// create pipeline
			s_Pipelines[file] = std::make_unique<Pipeline>(spv.value(), generateVulkanDescriptor(inputDescription));
			s_Pipelines[file]->Create(RenderPass::GetDefaultPass().GetHandle());

			return true;
		}
	}

	void ShaderLibrary::Remove(const std::string& file)
	{
		//TODO
		PR_CORE_WARN("Shader removal not implemented yet - {0} will be removed at shutdown", file);
	}



	// ============= Utility =================================

	std::optional<Shader::Code> readFile(const std::string& filepath)
	{
		Shader::Code result;

		// read file content
		std::string fileContent;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			fileContent.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&fileContent[0], fileContent.size());
			in.close();
		}
		else
		{
			PR_CORE_ERROR("Could not open file '{0}'", filepath);
			return std::nullopt;
		}

		// split content into shader sources
		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = fileContent.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			// read type
			size_t eol = fileContent.find_first_of("\r\n", pos);
			PR_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string typestr = fileContent.substr(begin, eol - begin);

			// parse type
			Shader::Type type;
			if (typestr == "vertex")						type = Shader::Type::Vertex;
			else if (typestr == "fragment")					type = Shader::Type::Fragment;
			else if (typestr == "compute")					type = Shader::Type::Compute;
			else if (typestr == "geometry")					type = Shader::Type::Geometry;
			else if (typestr == "tesselation control")		type = Shader::Type::TesselationControl;
			else if (typestr == "tesselation evaluation")	type = Shader::Type::TesselationEvaluation;
			else { PR_CORE_ASSERT(false, "Unknown shader type '{0}'", typestr); }

			// read shader source
			size_t nextLinePos = fileContent.find_first_not_of("\r\n", eol);
			pos = fileContent.find(typeToken, nextLinePos);
			result[type] = fileContent.substr(nextLinePos,
				pos - (nextLinePos == std::string::npos ? fileContent.size() - 1 : nextLinePos));
		}

		return result;
	}

	std::optional<Shader::SpirV> compile(const Shader::Code& code, const char* name)
	{
		Shader::SpirV result;
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		options.AddMacroDefinition("MY_DEFINE", "1");
		options.SetOptimizationLevel(shaderc_optimization_level_size);

		for (const auto& shader : code)
		{
			shaderc::SpvCompilationResult compiled = compiler.CompileGlslToSpv(
				shader.second, /* code */
				(shaderc_shader_kind)shader.first, /* type */
				name, options);

			if (compiled.GetCompilationStatus() != shaderc_compilation_status_success) {
				PR_CORE_ERROR("Error compiling shader '{0}': {1}", name, compiled.GetErrorMessage());
				return std::nullopt;
			}
			result[shader.first] = { compiled.cbegin(), compiled.cend() };
		}

		PR_CORE_TRACE("Shader '{0}' compiled", name);
		return result;
	}

	Vulkan::VertexBufferDescriptor generateVulkanDescriptor(const VertexInputDescription& genericDescriptor)
	{
		Vulkan::VertexBufferDescriptor result;

		constexpr uint32_t binding = 0;
		result.bindingDescription = vk::VertexInputBindingDescription(binding, genericDescriptor.stride);

		uint32_t attribCount = genericDescriptor.attributes.size();
		result.attributeDescriptions.resize(attribCount);

		for (uint32_t i = 0; i < attribCount; ++i)
		{
			result.attributeDescriptions[i] = vk::VertexInputAttributeDescription(i,
				binding,
				generateVulkanDataType(genericDescriptor.attributes[i].type),
				genericDescriptor.attributes[i].offset);
		}
		return result;
	}

	vk::Format generateVulkanDataType(const BufferDataType& dataType)
	{
		switch (dataType)
		{
		case BufferDataType::Float:    return vk::Format::eR32Sfloat;
		case BufferDataType::Float2:   return vk::Format::eR32G32Sfloat;
		case BufferDataType::Float3:   return vk::Format::eR32G32B32Sfloat;
		case BufferDataType::Float4:   return vk::Format::eR32G32B32A32Sfloat;
		case BufferDataType::Int:      return vk::Format::eR32Sint;
		case BufferDataType::Int2:     return vk::Format::eR32G32Sint;
		case BufferDataType::Int3:     return vk::Format::eR32G32B32Sint;
		case BufferDataType::Int4:     return vk::Format::eR32G32B32A32Sint;
		default:
			PR_CORE_ASSERT(false, "Cannot determine Vulkan Format for BufferDatatype");
		}
	}

}