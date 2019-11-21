#include "pch.h"
#include "VulkanSwapchain.h"

namespace Prism {
	// forward declarations
	vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);

	void VulkanSwapchain::Create(vk::Device device, vk::Extent2D size, vk::SurfaceKHR surface)
	{
		vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(supportDetails.formats);
		vk::PresentModeKHR presentMode = chooseSwapPresentMode(supportDetails.presentModes);

		// image count (minCount + 1 to triplebuffer)
		uint32_t imageCount = supportDetails.capabilities.minImageCount + 1;
		if (supportDetails.capabilities.maxImageCount > 0 &&
			imageCount > supportDetails.capabilities.maxImageCount)
		{
			imageCount = supportDetails.capabilities.maxImageCount;
		}

		vk::SwapchainCreateInfoKHR swapChainCreateInfo(
			vk::SwapchainCreateFlagsKHR(),
			surface,
			imageCount,
			surfaceFormat.format, surfaceFormat.colorSpace,
			size, 1, vk::ImageUsageFlagBits::eColorAttachment,
			vk::SharingMode::eExclusive, 0, nullptr,
			supportDetails.capabilities.currentTransform,
			vk::CompositeAlphaFlagBitsKHR::eOpaque,
			presentMode, true, nullptr);

		swapchain = device.createSwapchainKHR(swapChainCreateInfo);
		format = surfaceFormat.format;
		extent = size;

		// get Images from Swapchain
		images = device.getSwapchainImagesKHR(swapchain);

		imageViews.reserve(images.size());
		for (auto image : images)
			imageViews.push_back(CreateImageView(device, image));

		// if there's already a template render pass, create framebuffers (else call SetRenderPass)
		if (renderPass)
			CreateFrameBuffers(device);
	}

	void VulkanSwapchain::Recreate(vk::Device device, vk::Extent2D extent, vk::SurfaceKHR surface)
	{
		device.waitIdle();
		CleanUp(device);
		Create(device, extent, surface);
	}

	void VulkanSwapchain::CleanUp(vk::Device device)
	{
		for (const auto& framebuffer : framebuffers)
			device.destroyFramebuffer(framebuffer);
		framebuffers.clear();

		for (const auto& imageView : imageViews)
			device.destroyImageView(imageView);
		imageViews.clear();

		device.destroySwapchainKHR(swapchain);
	}

	void VulkanSwapchain::CreateFrameBuffers(vk::Device device)
	{
		PR_CORE_ASSERT(framebuffers.size() < 1, "There should be no Framebuffers at this point");

		framebuffers.reserve(imageViews.size());
		for (int i = 0; i < imageViews.size(); i++)
		{
			vk::ImageView attachments[] = {
				imageViews[i] // might be more
			};

			vk::FramebufferCreateInfo createInfo({},
				renderPass.value(),
				1, attachments,
				extent.width, extent.height, 1);

			framebuffers.emplace_back(device.createFramebuffer(createInfo));
		}
	}



	vk::ImageView VulkanSwapchain::CreateImageView(vk::Device device, vk::Image image)
	{
		vk::ImageViewCreateInfo createInfo({},
			image,
			vk::ImageViewType::e2D,
			format,
			vk::ComponentMapping(),
			vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)
		);

		return device.createImageView(createInfo);
	}

	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
	{
		PR_CORE_ASSERT(
			availableFormats.size() != 1 ||
			availableFormats[0].format != vk::Format::eUndefined,
			"Surface format undefined!"
		);

		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
				return availableFormat;
		}

		return availableFormats[0];
	}

	vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
	{
		vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;

		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == vk::PresentModeKHR::eMailbox)
				return availablePresentMode;
			else if (availablePresentMode == vk::PresentModeKHR::eImmediate)
				bestMode = availablePresentMode;
		}
		return bestMode;
	}
}