#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

namespace backend {

bool Mesh::LoadFromFile(const std::string& path) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace);
    if (!scene || !scene->HasMeshes()) {
        std::cerr << "[Mesh] Failed to load model: " << path << "\n";
        return false;
    }

    const aiMesh* mesh = scene->mMeshes[0];
    vertices.reserve(mesh->mNumVertices);

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        Vertex v{};
        v.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        v.normal = mesh->HasNormals() ? glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z) : glm::vec3(0.0f);
        v.uv = mesh->HasTextureCoords(0) ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : glm::vec2(0.0f);
        v.color = glm::vec3(1.0f); // Placeholder; will be overridden by material later
        vertices.push_back(v);
    }

    indices.reserve(mesh->mNumFaces * 3);
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        const aiFace& face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }

    std::cout << "[Mesh] Loaded mesh with " << vertices.size() << " vertices and " << indices.size() << " indices\n";
    return true;
}

void Mesh::UploadToGPU(VkDevice device, VkPhysicalDevice physical, VkCommandPool cmdPool, VkQueue queue) {
    vertexBuffer.Create(device, physical, sizeof(Vertex) * vertices.size(),
                        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    vertexBuffer.Upload(vertices.data(), sizeof(Vertex) * vertices.size());

    indexBuffer.Create(device, physical, sizeof(uint32_t) * indices.size(),
                       VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    indexBuffer.Upload(indices.data(), sizeof(uint32_t) * indices.size());

    std::cout << "[Mesh] Uploaded vertex and index buffers to GPU\n";
}

void Mesh::Destroy(VkDevice device) {
    vertexBuffer.Destroy(device);
    indexBuffer.Destroy(device);
}

}
