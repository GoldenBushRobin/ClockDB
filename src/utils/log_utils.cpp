#include "log_utils.h"
#include <chrono>
#include <format>

const char* timestamp() {
    return std::format("{:%F %T}", std::chrono::system_clock::now()).c_str()
}