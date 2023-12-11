#include "first_app.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <memory>

namespace little
{
    struct SimplePushConstantData
    {
        glm::mat2 transform{1.f};
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

    FirstApp::FirstApp()
    {
        loadGameObjects();
        createPipelineLayout();
        createPipeline();
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
            
            if (auto commandBuffer = littleRenderer.beginFrame())
            {
                littleRenderer.beginSwapChainRenderPass(commandBuffer);
                renderGameObjects(commandBuffer);
                littleRenderer.endSwapChainRenderPass(commandBuffer);
                littleRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(littleDevice.device());
    }

    void FirstApp::loadGameObjects()
    {
        std::vector<LittleModel::Vertex> vertices{
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

        auto littleModel = std::make_shared<LittleModel>(littleDevice, vertices);

        auto triangle = LittleGameObject::createGameObject();
        triangle.model = littleModel;
        triangle.color = {.1f, .8f, .1f};
        triangle.transform2d.translation.x = .0f;
        triangle.transform2d.scale = {.5f, .5f};
        triangle.transform2d.rotation = 0.25f * glm::two_pi<float>();

        gameObjects.push_back(std::move(triangle));
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
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        LittlePipeLine::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = littleRenderer.getSwapChainRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        littlePipeLine = std::make_unique<LittlePipeLine>(
            littleDevice,
            "shaders/simple_shader.vert.spv",
            "shaders/simple_shader.frag.spv",
            pipelineConfig);
    }

    void FirstApp::renderGameObjects(VkCommandBuffer commandBuffer)
    {
        littlePipeLine->bind(commandBuffer);

        for (auto &obj : gameObjects)
        {
            obj.transform2d.rotation = glm::mod(obj.transform2d.rotation + 0.01f, glm::two_pi<float>());

            SimplePushConstantData push{};
            push.offset = obj.transform2d.translation;
            push.color = obj.color;
            push.transform = obj.transform2d.mat2();

            vkCmdPushConstants(
                commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push);
            obj.model->bind(commandBuffer);
            obj.model->draw(commandBuffer);
        }
    }
}