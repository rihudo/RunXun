#pragma once
#include <stdio.h>

#define LOG_INFOR(format, ...) printf("[%s:%d]" format, __FILE__, __LINE__, ##__VA_ARGS__)
#define LOG_DEBUG LOG_INFOR
#define LOG_ERROR LOG_INFOR