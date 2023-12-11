#pragma once

#include "little_window.hpp"
#include "little_pipeline.hpp"
#include "little_device.hpp"
#include "little_swap_chain.hpp"
#include "little_game_object.hpp"

#include <memory>
#include <vector>

namespace little
{
    class FirstApp
    {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HIGHT = 600;

        FirstApp();
        ~FirstApp();

        FirstApp(const LittleWindow &) = delete;
        FirstApp &operator=(const LittleWindow &) = delete;

        void run();

    private:
        void loadGameObjects();
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void freeCommandBuffers();
        void drawFrame();
        void recreateSwapChain();
        void recordCommandBuffer(int imageIndex);
        void renderGameObjects(VkCommandBuffer commandBuffer);

        LittleWindow littleWindow{WIDTH, HIGHT, "Hello Vulkan!"};
        LittleDevice littleDevice{littleWindow};
        std::unique_ptr<LittleSwapChain> littleSwapChain;
        std::unique_ptr<LittlePipeLine> littlePipeLine;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<LittleGameObject> gameObjects;
    };
}