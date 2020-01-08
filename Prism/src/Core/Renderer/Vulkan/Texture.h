#pragma once

#include "vma/vk_mem_alloc.h"
#include "vulkan/vulkan.hpp"

#include "../Texture.h"
#include "CommandPool.h"

namespace Prism::Vulkan {

	struct Image
	{
		VkImage imageHandle;
		VkImageView imageView;
		VmaAllocation allocation;
		VkFormat format;

		uint32_t width, height;

		Image(VkImage image, VmaAllocation allocation, uint32_t width, uint32_t height, VkFormat format);

		void TransitionLayout(vk::CommandBuffer cmd, VkImageLayout oldLayout, VkImageLayout newLayout);

		~Image();
	};


	class Texture2D : public Prism::Texture2D
	{
	public:
		Texture2D(const std::string& file);
		const Image& GetImage() const { return *m_Image; };

	private:
		std::unique_ptr<Image> m_Image = nullptr;
	};
}