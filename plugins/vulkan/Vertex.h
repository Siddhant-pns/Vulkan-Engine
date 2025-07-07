#pragma once
#include <glm/glm.hpp>
#include <volk.h>
#include <array>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal; 
    glm::vec2 uv;
    glm::vec3 color;

    static VkVertexInputBindingDescription GetBindingDescription() {
        VkVertexInputBindingDescription binding{};
        binding.binding = 0;
        binding.stride = sizeof(Vertex);
        binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return binding;
    }

    static std::array<VkVertexInputAttributeDescription, 4> GetAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 4> attr{};

        attr[0].binding = 0;
        attr[0].location = 0;
        attr[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attr[0].offset = offsetof(Vertex, position);

        attr[1].binding = 0;
        attr[1].location = 1;
        attr[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attr[1].offset = offsetof(Vertex, normal);

        attr[2].binding = 0;
        attr[2].location = 2;
        attr[2].format = VK_FORMAT_R32G32_SFLOAT;
        attr[2].offset = offsetof(Vertex, uv);

        attr[3].binding = 0;
        attr[3].location = 3;
        attr[3].format = VK_FORMAT_R32G32B32_SFLOAT;
        attr[3].offset = offsetof(Vertex, color);

        return attr;
    }
};
