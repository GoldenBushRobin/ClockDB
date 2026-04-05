#include "log_utils.h"
#include <chrono>
#include <format>

std::string timestamp() {
    return std::format("{:%F %T}", std::chrono::system_clock::now());
}