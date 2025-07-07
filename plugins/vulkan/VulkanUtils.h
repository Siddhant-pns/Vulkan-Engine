#pragma once
#include <volk.h>
#include <string>

namespace backend {
    void CheckVkResult(VkResult result, const std::string& message);
}
