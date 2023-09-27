/**
 * @file debug.h
 * @author Frank Bender (fjbender2@gmail.com)
 * @brief Preprocessor directives responsible for managing terminal output for
 * various builds, i.e. Release vs. Debug. Build will be specified with CMake
 * flag in future versions.
 * @version 0.1 Alpha
 * @date 2023-09-20
 * 
 * @copyright Copyright © 2023 Jericho, LLC. All Rights Reserved.
 * 
 */

#ifndef COMET_DEBUG_H_
#define COMET_DEBUG_H_

// #define DEBUG
// #define TEST
#define ALPHA
// #define BETA
// #define RELEASE

#include "comet/util/io.h"

#ifdef DEBUG
#define DLOG(...) \
printf(__VA_ARGS__);
#else
#define DLOG(...)
#endif

#ifdef DEBUG
#define DLOGC(color, ...) \
    if (color == "gre") { \
        GRE(__VA_ARGS__); \
    } else if (color == "blu") { \
        BLU(__VA_ARGS__); \
    } else if (color == "red") { \
        RED(__VA_ARGS__); \
    } else if (color == "mag") { \
        MAG(__VA_ARGS__); \
    } else if (color == "cya") { \
        CYA(__VA_ARGS__); \
    } else if (color == "yel") { \
        YEL(__VA_ARGS__); \
    }
#else
#define DLOGC(color, ...)
#endif


#endif