#pragma once

#include <string>
#include <memory>

namespace Prism {

	class Texture2D
	{
	public:

		static std::shared_ptr<Texture2D> Create(const std::string& file);
		virtual ~Texture2D() = default;

	protected:
		uint32_t m_Width, m_Height;
	};
}