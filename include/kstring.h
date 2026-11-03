//
// Created by piti on 11/3/26.
//

#ifndef KORD_KSTRING_H
#define KORD_KSTRING_H

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

typedef struct kstring_s {
    char *buffer;  // the string
    size_t size;   // size of the buffer
    size_t length; // amount of characters
} kString;

static inline void kStrMake(kString* str) {
    str->buffer = nullptr;
    str->size = 0;
    str->length = 0;
}

// piti - resizes the buffer of a kString
static inline void kStrResize(kString *str, size_t newSize) {
    // no 0-byte alloc
    if (newSize == 0) newSize = 1;

    char *new_data = realloc(str->buffer, newSize);
    if (!new_data) {
        // handle alloc failure
        exit(1);
    }

    str->buffer = new_data;
    str->size = newSize;

    // ensures null termination
    if (str->length >= newSize) {
        str->length = newSize - 1;
        str->buffer[str->length] = '\0';
    }
}

// piti - appends other to str
static inline void kStrAppend(kString* str, const char* other) {
    if (str == nullptr) return;
    const size_t otherSize = strlen(other);
    if (otherSize + str->length >= str->size) {
        kStrResize(str, (size_t)((float)str->size * 1.5f) + otherSize);
    }

    memcpy(str->buffer + str->length, other, otherSize);

    str->length += otherSize;
    str->buffer[str->length] = '\0';
}

// piti - sets str to other
static inline void kStrSet(kString* str, const char* other) {
    if (str == nullptr) return;
    if (!other)
        other = "";
    const size_t otherSize = strlen(other);
    kStrResize(str, otherSize + 1);
    memcpy(str->buffer, other, otherSize);
    str->length = otherSize;
    str->buffer[str->length] = '\0';
}

// piti - compares kstr and cstr
[[nodiscard]]
static inline bool kStrEqualsC(kString* str, const char* other) {
    if (!other || !str) return false;
    return strcmp(str->buffer, other) == 0;
}

// piti - compares kstr and kstr
[[nodiscard]]
static inline bool kStrEquals(kString* str, kString* other) {
    if (!other || !str) return false;
    return strcmp(str->buffer, other->buffer) == 0;
}

// piti - duplicates a kstr
[[nodiscard]]
static inline kString kStrDup(const kString* str) {
    kString dupe;
    kStrMake(&dupe);
    if (str && str->buffer)
        kStrSet(&dupe, str->buffer);
    return dupe;
}


static inline void kStrDestroy(kString* str) {
    free(str->buffer);
    str->length = 0;
    str->size =   0;
}

#endif //KORD_KSTRING_H
