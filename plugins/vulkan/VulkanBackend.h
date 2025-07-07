#pragma once
#include "renderer/IRenderBackend.h"
#include <memory>

class VulkanBackend : public re::IRenderBackend {
public:
    VulkanBackend();
    ~VulkanBackend() override;

    void initialize() override;
    void drawFrame() override;
    void shutdown() override;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};
