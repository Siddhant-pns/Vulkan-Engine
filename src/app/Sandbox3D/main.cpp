#include "core/util/core.hpp"

#if defined(_WIN32)
#include <process.h>
#define GETPID _getpid
#else
#include <unistd.h>
#define GETPID getpid
#endif

int main() {
    using core::util::Logger;
    Logger::init("Sandbox3D");
    Logger::info("Skeleton build is running!  (pid=%d)", static_cast<int>(GETPID()));
    return 0;
}
