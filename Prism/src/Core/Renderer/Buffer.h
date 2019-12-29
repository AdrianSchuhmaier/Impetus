#pragma once

#include <vector>

namespace Prism {

	enum class BufferDataType {
		Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	struct IndexBuffer
	{
		virtual ~IndexBuffer() = default;
		static std::unique_ptr<IndexBuffer> Create(size_t, uint32_t* data);
	};

	struct VertexBuffer
	{
		struct Layout; // defined below

		virtual ~VertexBuffer() = default;
		static std::unique_ptr<VertexBuffer> Create(const VertexBuffer::Layout& inputDescription, size_t size, float* data);

		struct Layout
		{
			struct Attribute
			{
				uint32_t size;
				size_t offset;
				BufferDataType type;
			};

			uint32_t stride;
			std::vector<Attribute> attributes;


			// ====== Templated Layout Generation from structs ================

		private:
			template<typename Type>
			static constexpr BufferDataType getDataType(Type t)
			{
				if constexpr (std::is_same_v<Type, glm::float32>)
					return BufferDataType::Float;
				else if constexpr (std::is_same_v<Type, glm::vec2>)
					return BufferDataType::Float2;
				else if constexpr (std::is_same_v<Type, glm::vec3>)
					return BufferDataType::Float3;
				else if constexpr (std::is_same_v<Type, glm::vec4>)
					return BufferDataType::Float4;
				else if constexpr (std::is_same_v<Type, glm::mat3>)
					return BufferDataType::Mat3;
				else if constexpr (std::is_same_v<Type, glm::mat4>)
					return BufferDataType::Mat4;
				else
					static_assert(false, "VertexInput must contain only BufferDataTypes.");
			}

			template<typename S, typename Member, typename ...Members>
			static constexpr void inspectStruct(Attribute* result, Member x, Members... xs)
			{
				S s = S(); // create sample to measure offset
				*result = { sizeof(Member), (size_t) & (s.*x) - (size_t)&s, getDataType(s.*x) };

				if constexpr (sizeof...(xs) > 0)
					inspectStruct<S>(++result, xs...);
			}

		public:
			template<typename S, typename ...Ts>
			static constexpr VertexBuffer::Layout Generate(Ts... types)
			{
				VertexBuffer::Layout result{};
				result.stride = sizeof(S);
				result.attributes.resize(sizeof...(types));
				inspectStruct<S>(result.attributes.data(), types...);

				return result;
			}
		};
	};
}