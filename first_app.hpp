#pragma once

#include "little_window.hpp"
#include "little_pipeline.hpp"
#include "little_device.hpp"
#include "little_game_object.hpp"
#include "little_renderer.hpp"

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
        void renderGameObjects(VkCommandBuffer commandBuffer);

        LittleWindow littleWindow{WIDTH, HIGHT, "Hello Vulkan!"};
        LittleDevice littleDevice{littleWindow};
        LittleRenderer littleRenderer{littleWindow, littleDevice};

        std::unique_ptr<LittlePipeLine> littlePipeLine;
        VkPipelineLayout pipelineLayout;
        std::vector<LittleGameObject> gameObjects;
    };
}