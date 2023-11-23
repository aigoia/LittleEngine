#pragma once

#include "little_window.hpp"
#include "little_pipeline.hpp"
#include "little_device.hpp"
#include "little_swap_chain.hpp"
#include "little_model.hpp"

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
        void loadModels();
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void drawFrame();

        LittleWindow littleWindow{WIDTH, HIGHT, "Hello Vulkan!"};
        LittleDevice littleDevice{littleWindow};
        LittleSwapChain littleSwapChain{littleDevice, littleWindow.getExtent()};
        std::unique_ptr<LittlePipeLine> littlePipeLine;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
        std::unique_ptr<LittleModel> littleModel;
    };
}