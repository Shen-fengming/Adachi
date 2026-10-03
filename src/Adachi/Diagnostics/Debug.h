#pragma once

#include "Log.h"

#define ADACHI_CORE_ASSERT(condition, format, ...)                   \
    do                                                              \
    {                                                               \
        if (!(condition))                                           \
        {                                                           \
            ADACHI_CORE_ERROR(                                       \
                "Assertion failed: " format " [{}:{}]"              \
                __VA_OPT__(,) __VA_ARGS__, __FILE__, __LINE__);     \
            ADACHI_DEBUGBREAK();                                    \
        }                                                           \
    } while (false)

#define ADACHI_APP_ASSERT(condition, format, ...)                   \
    do                                                              \
    {                                                               \
        if (!(condition))                                           \
        {                                                           \
            ADACHI_APP_ERROR(                                       \
                "Assertion failed: " format " [{}:{}]"              \
                __VA_OPT__(,) __VA_ARGS__, __FILE__, __LINE__);     \
            ADACHI_DEBUGBREAK();                                    \
        }                                                           \
    } while (false)

// #define ADACHI_APP_ASSERT(condition, message)             \
//     do                                              \
//     {                                               \
//         if (!(condition))                           \
//         {                                           \
//             ADACHI_APP_ERROR("Assertion failed: {} {}, {}", message, __FILE__, __LINE__); \
//             ADACHI_DEBUGBREAK();                    \
//         }                                           \
//     } while (false)

#if defined(_MSC_VER)
    #define ADACHI_DEBUGBREAK() __debugbreak()
#elif defined(__clang__)
    #define ADACHI_DEBUGBREAK() __builtin_debugtrap()
#elif defined(__GNUC__)
    #include <csignal>
    #define ADACHI_DEBUGBREAK() raise(SIGTRAP)
#else
    #define ADACHI_DEBUGBREAK() ((void)0)
#endif