#pragma once
#include "Core/Window/Window.h"

#include "GLFW/glfw3.h"
#include "vulkan/vulkan.hpp"

#include "Swapchain.h"


namespace Prism::Vulkan {

	class Context
	{
	public:
		struct Queue { vk::Queue queue; uint32_t familyIndex = 0; };

	public:
		static void Init(const Window* window);
		static void CleanUp();

		// completely discards parameters at the moment!
		static void Resize(uint32_t width, uint32_t height)
		{
			int width_, height_;
			glfwGetFramebufferSize(m_WindowHandle, &width_, &height_);
			m_Swapchain.Recreate(m_Device, 
				{ static_cast<uint32_t>(width_), static_cast<uint32_t>(height_) },
				m_Surface.get());
		}

		static vk::PhysicalDevice GetPhysicalDevice() { return m_PhysicalDevice; }
		static vk::Device GetDevice() { return m_Device; }
		static Queue& GetGraphicsQueue() { return m_GraphicsQueue; }
		static Queue& GetTransferQueue() { return m_TransferQueue; }
		static Swapchain& GetSwapchain() { return m_Swapchain; }

	private:
		static GLFWwindow* m_WindowHandle;

		static vk::UniqueInstance m_Instance;
		static vk::UniqueSurfaceKHR m_Surface;
		static vk::PhysicalDevice m_PhysicalDevice;
		static vk::Device m_Device;

		static Queue m_GraphicsQueue;
		static Queue m_TransferQueue;

		static Swapchain m_Swapchain;

	private:
		static vk::UniqueInstance CreateInstance();
		static vk::UniqueSurfaceKHR CreateSurface();
		static std::pair<vk::Device, vk::PhysicalDevice> SelectDeviceAndQueues();
	};
}