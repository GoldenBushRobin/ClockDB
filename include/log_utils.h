#pragma once

#include <stdio.h>

enum log_levels { DEBUG, INFO, WARNING, ERROR, CRITICAL };
const char* log_level_strs[] = {"DEBUG","INFO","WARNING","ERROR","CRITICAL"};
const char* timestamp();

#define LOG(LOG_LEVEL, fmt, ...) fprintf(stderr, "[%s] %s: " fmt "\n", timestamp(), log_level_strs[LOG_LEVEL] __VA_OPT__(,) __VA_ARGS__)