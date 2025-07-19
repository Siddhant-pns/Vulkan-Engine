#pragma once
#include <cstdio>
#include <string_view>

namespace core::util {

enum class LogLevel { Info, Warn, Error };

class Logger {
  public:
    static void init(std::string_view tag = "ULT_GFX") noexcept {
        s_tag = tag;
    }

    template <typename... Args> static void info(std::string_view fmt, Args&&... args) noexcept {
        log(LogLevel::Info, fmt, std::forward<Args>(args)...);
    }
    template <typename... Args> static void warn(std::string_view fmt, Args&&... args) noexcept {
        log(LogLevel::Warn, fmt, std::forward<Args>(args)...);
    }
    template <typename... Args> static void error(std::string_view fmt, Args&&... args) noexcept {
        log(LogLevel::Error, fmt, std::forward<Args>(args)...);
    }

  private:
    template <typename... Args> static void log(LogLevel lvl, std::string_view fmt, Args&&... args) noexcept {
        const char* prefix = (lvl == LogLevel::Info) ? "[I]" : (lvl == LogLevel::Warn) ? "[W]" : "[E]";
        std::fprintf(stderr, "%s %s: ", prefix, s_tag.data());
        std::fprintf(stderr, fmt.data(), std::forward<Args>(args)...);
        std::fputc('\n', stderr);
    }
    inline static std::string_view s_tag = "ULT_GFX";
};

} // namespace core::util
