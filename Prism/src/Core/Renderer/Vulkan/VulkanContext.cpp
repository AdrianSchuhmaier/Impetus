#include "pch.h"
#include "VulkanContext.h"

#include <map>

namespace Prism {

	GLFWwindow* VulkanContext::m_WindowHandle = nullptr;

	vk::UniqueInstance VulkanContext::m_Instance;
	vk::UniqueSurfaceKHR VulkanContext::m_Surface;
	vk::PhysicalDevice VulkanContext::m_PhysicalDevice;
	vk::Device VulkanContext::m_Device;

	VulkanContext::Queue VulkanContext::m_GraphicsQueue;
	VulkanContext::Queue VulkanContext::m_TransferQueue;
	VulkanSwapchain VulkanContext::m_Swapchain;

	constexpr const char* VALIDATION_LAYER = "VK_LAYER_LUNARG_standard_validation";
	constexpr const char* SWAPCHAIN_EXT = "VK_KHR_swapchain";
	bool useValidation = false;

	void VulkanContext::Init(const Window* window)
	{
		m_WindowHandle = static_cast<GLFWwindow*>(window->GetWindowHandle());
		PR_CORE_ASSERT(m_WindowHandle, "Cannot find GLFWwindow handle!");

		m_Instance = CreateInstance();
		m_Surface = CreateSurface();

		auto& [device_, physicalDevice_] = SelectDeviceAndQueues();
		m_Device = device_; m_PhysicalDevice = physicalDevice_;
		
		// side-effects have set queues and m_Swapchain.supportDetails!

		int width, height;
		glfwGetFramebufferSize(m_WindowHandle, &width, &height);

		m_Swapchain.Create(m_Device,
			{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) },
			m_Surface.get());
	}

	void VulkanContext::CleanUp()
	{
		m_Device.waitIdle();
		m_Swapchain.CleanUp(m_Device);
		m_Device.destroy();
		PR_CORE_TRACE("VulkanContext destructed.");
	}

	vk::UniqueInstance VulkanContext::CreateInstance()
	{
		vk::ApplicationInfo appInfo(
			"Prism", VK_MAKE_VERSION(1, 0, 0), // Application
			"Prism", VK_MAKE_VERSION(1, 0, 0), // Engine
			VK_API_VERSION_1_1
		);

		// query required extensions
		uint32_t requiredCount;
		auto requiredRaw = glfwGetRequiredInstanceExtensions(&requiredCount);
		std::vector <const char*> required(requiredRaw, requiredRaw + requiredCount);

		// query available extensions
		auto availableExtensions = vk::enumerateInstanceExtensionProperties();
		std::vector<const char*> available(availableExtensions.size());
		std::transform(
			availableExtensions.begin(), availableExtensions.end(), available.begin(),
			[](const vk::ExtensionProperties& ex) { return ex.extensionName; });

#ifdef PR_DEBUG
		required.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		// check if all required extensions are available
		for (const auto& r : required)
			PR_CORE_ASSERT([&]() {
			for (const auto& a : available)
				if (strcmp(a, r))
					return true;
			return false; }(),
				"Vulkan extension missing: {0}", r);

		// validation layers
		auto availableLayers = vk::enumerateInstanceLayerProperties();
		std::vector<const char*> availableL(availableLayers.size());
		std::transform(availableLayers.begin(), availableLayers.end(), availableL.begin(),
			[](const vk::LayerProperties& l) { return l.layerName; });
		useValidation = [&]() {
			for (const auto& l : availableL)
				if (strcmp(l, VALIDATION_LAYER))
					return true;
			return false;
		}();
		PR_CORE_ASSERT(useValidation, "Validation layer not found, but required in debug mode.");
#endif

		vk::InstanceCreateInfo createInfo({}, &appInfo,
			static_cast<uint32_t>(useValidation), & VALIDATION_LAYER,
			static_cast<uint32_t>(required.size()), required.data());

		return vk::createInstanceUnique(createInfo);
	}

	vk::UniqueSurfaceKHR VulkanContext::CreateSurface()
	{
		std::vector<char const*> extensions;

		VkSurfaceKHR surface_;
		int res = glfwCreateWindowSurface(m_Instance.get(),
			m_WindowHandle,
			nullptr, &surface_);
		PR_CORE_ASSERT(!res, "GLFW could not create a Vulkan surface");

		return vk::UniqueSurfaceKHR(surface_,
			vk::ObjectDestroy<vk::Instance, vk::DispatchLoaderStatic>(m_Instance.get())
		);
	}

	std::pair<vk::Device, vk::PhysicalDevice> VulkanContext::SelectDeviceAndQueues()
	{
		PR_CORE_INFO("Selecting graphics device to use for Vulkan");
		// query for available
		auto availableDevices = m_Instance->enumeratePhysicalDevices();
		PR_CORE_ASSERT(!availableDevices.empty(), "No physical graphics device found");

		// Save devices in a ordered (descending) map
		std::map<int, VkPhysicalDevice, std::greater<int>> devices;

		// list devices and sort them based on a score
		PR_CORE_TRACE("Graphics devices found: ");
		for (const auto& device : availableDevices) {
			// check for swapchain extension
			bool hasSwapChainExtension = false;
			for (const auto& extensionProperty : device.enumerateDeviceExtensionProperties())
			{
				if (!strcmp(extensionProperty.extensionName, SWAPCHAIN_EXT))
				{
					hasSwapChainExtension = true;
					break;
				}
			}
			if (!hasSwapChainExtension)
				continue;

			// evaluate properties (to a score to sort by)
			auto props = device.getProperties();
			int score =
				// affects image quality
				props.limits.maxImageDimension2D
				// bonus for discrete GPU (performance)
				+ 4000 * (props.deviceType == vk::PhysicalDeviceType::eDiscreteGpu);

			PR_CORE_TRACE("\t{0}", props.deviceName);
			devices.insert(std::make_pair(score, device));
		}
		PR_CORE_ASSERT(!devices.empty(), "No devices found!");

		// iterate through devices (beginning with the highest score)
		for (const auto& [_, physicalDevice_] : devices) {
			const vk::PhysicalDevice& physicalDevice = physicalDevice_;

			// get the QueueFamilyProperties of the first PhysicalDevice
			auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
			const uint32_t availableQueueFamilyCount = queueFamilyProperties.size();
			uint32_t graphicsQueueFamilyIndex = availableQueueFamilyCount;
			uint32_t transferQueueFamilyIndex = availableQueueFamilyCount;

			// get graphics queue with present capability
			for (uint32_t i = 0; i < queueFamilyProperties.size(); ++i)
				if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics
					&& physicalDevice.getSurfaceSupportKHR(i, m_Surface.get()))
				{
					graphicsQueueFamilyIndex = i; break;
				}

			// if device doesn't have a queue family supporting both graphics and present, try the next one
			if (graphicsQueueFamilyIndex >= availableQueueFamilyCount) continue;

			// try to get transfer queue family separate from the others
			for (uint32_t i = 0; i < queueFamilyProperties.size(); ++i)
				if (i != graphicsQueueFamilyIndex && queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eTransfer)
				{
					transferQueueFamilyIndex = i; break;
				}

			uint32_t transferQueueIndex = 0;
			// if no separate queue family found, take graphics family with different queue if possible
			if (transferQueueFamilyIndex >= availableQueueFamilyCount)
			{
				transferQueueFamilyIndex = graphicsQueueFamilyIndex;
				if (queueFamilyProperties[transferQueueFamilyIndex].queueCount > 1)
					transferQueueIndex = 1;
			}

			float queuePriority = 0.0f;
			// prepare queue(s) info for creating the logical device
			vk::DeviceQueueCreateInfo deviceQueueCreateInfo[] = {
				vk::DeviceQueueCreateInfo({}, graphicsQueueFamilyIndex, 1 + transferQueueIndex,&queuePriority),
				vk::DeviceQueueCreateInfo({}, transferQueueFamilyIndex, 1,&queuePriority)
			};

			// create device information (debug includes Validation layers)
			vk::DeviceCreateInfo createInfo({},
				2 - transferQueueIndex, deviceQueueCreateInfo,
				static_cast<uint32_t>(useValidation), & VALIDATION_LAYER,
				1, & SWAPCHAIN_EXT, /* device features */ nullptr);

			// set queue member variables (they are needed to draw)
			vk::Device device = physicalDevice.createDevice(createInfo);
			m_GraphicsQueue = { device.getQueue(graphicsQueueFamilyIndex, 0), graphicsQueueFamilyIndex };
			m_TransferQueue = { device.getQueue(transferQueueFamilyIndex, transferQueueIndex), transferQueueFamilyIndex };

			// get swapchain support (device dependent)
			m_Swapchain.supportDetails.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(m_Surface.get());
			m_Swapchain.supportDetails.formats = physicalDevice.getSurfaceFormatsKHR(m_Surface.get());
			m_Swapchain.supportDetails.presentModes = physicalDevice.getSurfacePresentModesKHR(m_Surface.get());

			PR_CORE_ASSERT(!m_Swapchain.supportDetails.formats.empty(), "No formats supported.");
			PR_CORE_ASSERT(!m_Swapchain.supportDetails.presentModes.empty(), "No present modes supported.");

			return std::pair<vk::Device, vk::PhysicalDevice>(device, physicalDevice);
		}
		PR_CORE_ASSERT(false, "No suitable device for Vulkan found!");
	}

}