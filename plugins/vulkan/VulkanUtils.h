#pragma once
#include <string>
#include <volk.h>

namespace backend {
void CheckVkResult(VkResult result, const std::string& message);
}
