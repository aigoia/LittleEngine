#include "little_renderer.hpp"

#include <stdexcept>
#include <array>
#include <memory>

namespace little
{
    LittleRenderer::LittleRenderer(LittleWindow &window, LittleDevice &device) : littleWindow{window}, littleDevice{device}
    {
        recreateSwapChain();
        createCommandBuffers();
    }

    LittleRenderer::~LittleRenderer()
    {
        freeCommandBuffers();
    }

    void LittleRenderer::recreateSwapChain()
    {
        auto extent = littleWindow.getExtent();
        while (extent.width == 0 || extent.height == 0)
        {
            extent = littleWindow.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(littleDevice.device());

        if (littleSwapChain == nullptr)
        {
            littleSwapChain = std::make_unique<LittleSwapChain>(littleDevice, extent);
        }
        else
        {
            littleSwapChain = std::make_unique<LittleSwapChain>(littleDevice, extent, std::move(littleSwapChain));

            if (littleSwapChain->imageCount() != commandBuffers.size())
            {
                freeCommandBuffers();
                createCommandBuffers();
            }
        }

        // we'll come back to this in just a moment
    }

    void LittleRenderer::createCommandBuffers()
    {
        commandBuffers.resize(littleSwapChain->imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = littleDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(littleDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    void LittleRenderer::freeCommandBuffers()
    {
        vkFreeCommandBuffers(
            littleDevice.device(),
            littleDevice.getCommandPool(),
            static_cast<float>(commandBuffers.size()),
            commandBuffers.data());

        commandBuffers.clear();
    }

    VkCommandBuffer LittleRenderer::beginFrame()
    {
        assert(!isFrameStarted && "Cannot call beginFrame while already in progress");

        auto result = littleSwapChain->acquireNextImage(&currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapChain();
            return nullptr;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        isFrameStarted = true;

        auto commandBuffer = getCurrentCommandBuffer();

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        return commandBuffer;
    }

    void LittleRenderer::endFrame()
    {
        assert(isFrameStarted && "Cannot call endFrame while frame is not progress");

        auto commandBuffer = getCurrentCommandBuffer();

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }

        auto result = littleSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || littleWindow.wasWindowResized())
        {
            littleWindow.resetWindowResizedFlag();
            recreateSwapChain();
        }

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image!");
        }

        isFrameStarted = false;
    };

    void LittleRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "Cannot call beginSwapChainRenderPass if frame is not in progress");
        assert(
            commandBuffer == getCurrentCommandBuffer() &&
            "Cannot begin render pass on command buffer from a different frame");

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = littleSwapChain->getRenderPass();
        renderPassInfo.framebuffer = littleSwapChain->getFrameBuffer(currentImageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = littleSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.1f, 0.1f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(littleSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(littleSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, littleSwapChain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void LittleRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "Cannot call endSwapChainRenderPass if frame is not in progress");
        assert(
            commandBuffer == getCurrentCommandBuffer() &&
            "Cannot end render pass on command buffer from a different frame");

        vkCmdEndRenderPass(commandBuffer);
    }

}