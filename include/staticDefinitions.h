//
// Created by piti on 11/3/26.
//

#ifndef KORD_STATIC_DEFINITIONS_H
#define KORD_STATIC_DEFINITIONS_H

#define KORD_VERSION_MAJOR 0
#define KORD_VERSION_PATCH 0
#define KORD_VERSION_MINOR 0

// num to str
#define KORD_STR2(x) #x
#define KORD_STR(x) KORD_STR2(x)

// version string from numbers
#define KORD_VERSION_STRING \
KORD_STR(KORD_VERSION_MAJOR) "." \
KORD_STR(KORD_VERSION_PATCH) "." \
KORD_STR(KORD_VERSION_MINOR)

// controls
const char* kC_reset      = "\033[0m";
const char* kC_clear      = "\033[2J\033[H";

// styles
const char* kC_bold       = "\033[1m";
const char* kC_underline  = "\033[4m";
const char* kC_italic     = "\033[3m";

// foreground
const char* kC_black      = "\033[30m";
const char* kC_red        = "\033[31m";
const char* kC_green      = "\033[32m";
const char* kC_yellow     = "\033[33m";
const char* kC_blue       = "\033[34m";
const char* kC_magenta    = "\033[35m";
const char* kC_cyan       = "\033[36m";
const char* kC_white      = "\033[37m";

// foreground bright
const char* kC_b_black    = "\033[90m";
const char* kC_b_red      = "\033[91m";
const char* kC_b_green    = "\033[92m";
const char* kC_b_yellow   = "\033[93m";
const char* kC_b_blue     = "\033[94m";
const char* kC_b_magenta  = "\033[95m";
const char* kC_b_cyan     = "\033[96m";
const char* kC_b_white    = "\033[97m";

// background
const char* kC_bg_black   = "\033[40m";
const char* kC_bg_red     = "\033[41m";
const char* kC_bg_green   = "\033[42m";
const char* kC_bg_yellow  = "\033[43m";
const char* kC_bg_blue    = "\033[44m";
const char* kC_bg_magenta = "\033[45m";
const char* kC_bg_cyan    = "\033[46m";
const char* kC_bg_white   = "\033[47m";

#endif //KORD_STATIC_DEFINITIONS_H