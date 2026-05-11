#pragma once

#ifdef DEBUG
#include <print>
#define GREEN  "\033[0;32m"
#define YELLOW "\033[0;33m"
#define RED    "\033[0;31m"
#define RESET  "\033[0;0m"

#define LOG(fmt, ...) std::println(GREEN "[INFO {}:{}]" fmt RESET,  __FILE__, __FUNCTION__, ##__VA_ARGS__)
#define WARN(fmt, ...) std::println(YELLOW "[WARN {}:{}]" fmt RESET, __FILE__, __FUNCTION__, ##__VA_ARGS__)
#define ERR(fmt, ...) do { std::println(RED "[ERR {}:{}]" fmt RESET, __FILE__, __FUNCTION__, ##__VA_ARGS__); exit(1); } while(0);

#else

#define LOG(fmt, ...)
#define WARN(fmt, ...)
#define ERR(fmt, ...)

#endif
