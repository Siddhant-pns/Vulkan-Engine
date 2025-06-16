#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "scene/Vertex.h"
#include "backend/VulkanBuffer.h"

namespace backend {

    class Mesh {
    public:
        bool LoadFromFile(const std::string& path);
        void UploadToGPU(VkDevice device, VkPhysicalDevice physical, VkCommandPool cmdPool, VkQueue queue);
        void Destroy(VkDevice device);

        VkBuffer GetVertexBuffer() const { return vertexBuffer.Get(); }
        VkBuffer GetIndexBuffer() const { return indexBuffer.Get(); }
        uint32_t GetIndexCount() const { return static_cast<uint32_t>(indices.size()); }

    private:
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        VulkanBuffer vertexBuffer;
        VulkanBuffer indexBuffer;
    };

}
