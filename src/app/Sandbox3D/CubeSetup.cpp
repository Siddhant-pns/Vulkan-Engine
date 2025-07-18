#include "CubeSetup.hpp"
#include "graphics/data/CubeVerts.hpp"
#include <iostream>


CubeResources cube;

bool InitCube(const backend::VulkanDevice& dev,
              VkCommandPool pool, VkQueue q,
              const backend::VulkanSwapchain& sw)
{

    const VkMemoryPropertyFlags cpuVisible =
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    std::cout <<"pool" << pool << " q " << q << std::endl;
    /* buffers */
    cube.vb.Create(dev.logical(),dev.physical(),sizeof(kCubeVerts),
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        cpuVisible);
    cube.ib.Create(dev.logical(),dev.physical(),sizeof(kCubeIdx),
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        cpuVisible);
    cube.vb.Upload(kCubeVerts, sizeof(kCubeVerts),
                pool, dev.GetGraphicsQueue());
    cube.ib.Upload(kCubeIdx, sizeof(kCubeIdx),
                pool, dev.GetGraphicsQueue());


    /* texture */
    cube.tex.LoadFromFile(dev.logical(),dev.physical(),
                          pool,q,"assets/textures/checker.png");

    /* descriptor + UBO */
    cube.desc.Create(dev.logical());
    cube.desc.Allocate(dev.logical());
    cube.desc.Update(dev.logical(),cube.tex.GetImageView(),cube.tex.GetSampler());

    cube.ubo.Create(dev.logical(),dev.physical(),
                    sizeof(glm::mat4), sw.GetImageViews().size());
    cube.desc.Update(dev.logical(), cube.ubo.GetDescriptorInfo(0));

    /* shaders & pipeline */
    backend::VulkanShader vs,fs;
    vs.LoadFromFile(dev.logical(),"shaders/cube.vert.spv");
    fs.LoadFromFile(dev.logical(),"shaders/cube.frag.spv");

    cube.pipe.CreateRenderPass(dev.logical(),sw.GetFormat(),sw.GetDepthFormat());
    cube.pipe.CreateFramebuffers(dev.logical(), sw.GetExtent(),
                                 sw.GetImageViews(), sw.GetDepthImageView());
    cube.pipe.CreateGraphicsPipeline(dev.logical(), sw.GetExtent(),
                                     cube.pipe.GetRenderPass(),
                                     vs.Get(), fs.Get(),
                                     cube.desc.GetLayout0(),
                                     cube.desc.GetLayout1());

    vs.Destroy(dev.logical()); fs.Destroy(dev.logical());
    return true;
}

void DestroyCube(const backend::VulkanDevice& dev)
{
    cube.pipe.Destroy(dev.logical());       // framebuffer, render-pass, pipeline
    cube.desc.Destroy(dev.logical());       // sets, pool, layouts
    cube.ubo.Destroy(dev.logical());        // buffers, mem
    cube.tex.Destroy(dev.logical());        // view, sampler, image, mem
    cube.ib.Destroy(dev.logical());         // buffer + mem
    cube.vb.Destroy(dev.logical());
}
