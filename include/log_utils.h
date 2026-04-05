#pragma once

#include <stdio.h>
#include <string>

enum log_levels { DEBUG, INFO, WARNING, ERROR, CRITICAL };
inline const char* log_level_strs[] = {"DEBUG","INFO","WARNING","ERROR","CRITICAL"};
std::string timestamp();

#define LOG(LOG_LEVEL, fmt, ...) fprintf(stderr, "[%s] %s: " fmt "\n", timestamp().c_str(), log_level_strs[LOG_LEVEL] __VA_OPT__(,) __VA_ARGS__)