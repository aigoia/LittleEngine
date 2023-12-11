#pragma once

#include "little_window.hpp"
#include "little_device.hpp"
#include "little_swap_chain.hpp"

#include <cassert>
#include <memory>
#include <vector>

namespace little
{
    class LittleRenderer
    {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HIGHT = 600;

        LittleRenderer(LittleWindow &window, LittleDevice &device);
        ~LittleRenderer();

        LittleRenderer(const LittleWindow &) = delete;
        LittleRenderer &operator=(const LittleWindow &) = delete;

        VkRenderPass getSwapChainRenderPass() const { return littleSwapChain->getRenderPass(); }
        bool isFrameInProgress() const { return isFrameStarted; }

        VkCommandBuffer getCurrentCommandBuffer() const {
            assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
            return commandBuffers[currentImageIndex];
        }

        VkCommandBuffer beginFrame();
        void endFrame();
        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

    private:
        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapChain();

        LittleWindow &littleWindow;
        LittleDevice &littleDevice;
        std::unique_ptr<LittleSwapChain> littleSwapChain;
        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex;
        bool isFrameStarted;
    };
}