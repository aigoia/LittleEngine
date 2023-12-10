#include "first_app.hpp"

#include <stdexcept>
#include <array>
#include <memory>

namespace little
{
    struct SimplePushConstantData
    {
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

    FirstApp::FirstApp()
    {
        loadModels();
        createPipelineLayout();
        recreateSwapChain();
        createCommandBuffers();
    }

    FirstApp::~FirstApp()
    {
        vkDestroyPipelineLayout(littleDevice.device(), pipelineLayout, nullptr);
    }

    void FirstApp::run()
    {
        while (!littleWindow.shouldClose())
        {
            glfwPollEvents();
            drawFrame();
        }

        vkDeviceWaitIdle(littleDevice.device());
    }

    void FirstApp::loadModels()
    {
        std::vector<LittleModel::Vertex> vertices{
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

        littleModel = std::make_unique<LittleModel>(littleDevice, vertices);
    }

    void FirstApp::createPipelineLayout()
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(littleDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void FirstApp::createPipeline()
    {
        PipelineConfigInfo pipelineConfig{};
        LittlePipeLine::defaultPipelineConfigInfo(
            pipelineConfig);

        pipelineConfig.renderPass = littleSwapChain->getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        littlePipeLine = std::make_unique<LittlePipeLine>(
            littleDevice,
            "shaders/simple_shader.vert.spv",
            "shaders/simple_shader.frag.spv",
            pipelineConfig);
    }

    void FirstApp::recreateSwapChain()
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

        createPipeline();
    }

    void FirstApp::createCommandBuffers()
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

    void FirstApp::freeCommandBuffers()
    {
        vkFreeCommandBuffers(
            littleDevice.device(),
            littleDevice.getCommandPool(),
            static_cast<float>(commandBuffers.size()),
            commandBuffers.data());

        commandBuffers.clear();
    }

    void FirstApp::recordCommandBuffer(int imageIndex)
    {
        static int frame = 0;
        frame = (frame + 1) % 1000;

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = littleSwapChain->getRenderPass();
        renderPassInfo.framebuffer = littleSwapChain->getFrameBuffer(imageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = littleSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.1f, 0.1f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(littleSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(littleSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, littleSwapChain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

        littlePipeLine->bind(commandBuffers[imageIndex]);
        littleModel->bind(commandBuffers[imageIndex]);

        for (int j = 0; j < 4; j++)
        {
            SimplePushConstantData push{};
            push.offset = {-0.5f + frame * 0.002f, -0.4f + j * 0.25f};
            push.color = {0.0f, 0.0f, 0.2f + 0.2f * j};

            vkCmdPushConstants(
                commandBuffers[imageIndex],
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push);

            littleModel->draw(commandBuffers[imageIndex]);
        }

        vkCmdEndRenderPass(commandBuffers[imageIndex]);
        if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void FirstApp::drawFrame()
    {
        uint32_t imageIndex;
        auto result = littleSwapChain->acquireNextImage(&imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapChain();
            return;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        recordCommandBuffer(imageIndex);
        result = littleSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || littleWindow.wasWindowResized())
        {
            littleWindow.resetWindowResizedFlag();
            recreateSwapChain();
        }

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image!");
        }
    }
}