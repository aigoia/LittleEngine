#pragma once

#include "little_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace little
{
    class LittleModel
    {
    public:
        struct Vertex
        {
            glm::vec2 position;
            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

        LittleModel(LittleDevice &device, const std::vector<Vertex> &vertices);
        ~LittleModel();

        LittleModel(const LittleWindow &) = delete;
        LittleModel &operator=(const LittleWindow &) = delete;

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);

    private:
        void createVertexBuffers(const std::vector<Vertex> &vertices);

        LittleDevice &littleDevice;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        uint32_t vertexCount;
    };
}