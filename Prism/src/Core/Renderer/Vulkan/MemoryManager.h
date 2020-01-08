#pragma once
#include "vulkan/vulkan.hpp"
#include "vma/vk_mem_alloc.h"

#include "Buffer.h"
#include "Texture.h"
#include "CommandPool.h"

namespace Prism::Vulkan {

    class MemoryManager
    {
    public:

        static void Init();
        static void CleanUp();

        /*
        Allocates a buffer of the given type either on Host or Device memory (depending on access pattern).
        If data != nullptr, the data is copied into the buffer.
        */
        static std::unique_ptr<Buffer> CreateBuffer(size_t size, BufferType type, BufferAccessPattern access);
        static std::unique_ptr<Image> CreateImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage);

        static void BufferData(size_t size, void* data, const Buffer& buffer, uint32_t offset = 0);
        static void BufferImage(size_t size, void* data, Image& image);

        static void FreeBuffer(Buffer* buffer);
        static void FreeImage(Image* image);

    private:
        static VmaAllocator m_Allocator;

        static CommandPool m_CommandPool;
        static std::vector<uint32_t> m_QueueFamilyIndices;
    };
}