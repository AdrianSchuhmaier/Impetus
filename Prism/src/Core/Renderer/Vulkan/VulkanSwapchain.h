#pragma once

#include "vulkan/vulkan.hpp"

namespace Prism {

	struct VulkanSwapchain
	{
		struct {
			vk::SurfaceCapabilitiesKHR capabilities;
			std::vector<vk::SurfaceFormatKHR> formats;
			std::vector<vk::PresentModeKHR> presentModes;
		} supportDetails;

		vk::SwapchainKHR swapchain;
		vk::Format format;
		vk::Extent2D extent;

		std::vector<vk::Image> images;
		std::vector<vk::ImageView> imageViews;
		std::vector<vk::Framebuffer> framebuffers;
		std::vector<vk::CommandBuffer> commandBuffers;

		std::optional<vk::RenderPass> renderPass;

		// shall be called with supportDetails filled in!
		void Create(vk::Device device, vk::Extent2D extent, vk::SurfaceKHR surface);
		void Recreate(vk::Device device, vk::Extent2D extent, vk::SurfaceKHR surface);
		void CleanUp(vk::Device device);

		void CreateFrameBuffers(vk::Device device);
		vk::ImageView CreateImageView(vk::Device device, vk::Image image);
	};
}