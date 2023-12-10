#pragma once

#include "little_device.hpp"

#include <string>
#include <vector>

namespace little
{
    struct PipelineConfigInfo
    {
        PipelineConfigInfo() = default;
        PipelineConfigInfo(const PipelineConfigInfo &) = default;
        PipelineConfigInfo &operator=(const PipelineConfigInfo &) = default;

        VkViewport viewport;
        VkRect2D scissor;
        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

    class LittlePipeLine
    {
    public:
        LittlePipeLine(
            LittleDevice &device,
            const std::string &vertFilepath,
            const std::string &fragFilepath,
            const PipelineConfigInfo configInfo);
        ~LittlePipeLine();

        LittlePipeLine(const LittlePipeLine &) = delete;
        LittlePipeLine &operator=(const LittlePipeLine &) = delete;

        void bind(VkCommandBuffer commandBuffer);

        static void defaultPipelineConfigInfo(PipelineConfigInfo &configInfo, uint32_t width, uint32_t height);

    private:
        static std::vector<char> readFile(const std::string filepath);

        void createGraphicsPipeline(
            const std::string &vertFilepath,
            const std::string &fragFilepath,
            const PipelineConfigInfo configInfo);

        void createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);

        LittleDevice &littleDevice;
        VkPipeline graphicsPipeline;
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;
    };
}