//
// Created by piti on 11/10/26.
//

#ifndef KORD_KMEMORY_H
#define KORD_KMEMORY_H

#include <malloc.h>
#include <kvariant.h>

#include "kerror.h"

typedef struct {
    kString key;
    kVariant value;
} kRuntimeMemField;

typedef struct {
    kRuntimeMemField *memory;
    size_t length;
    size_t size;
} kRuntimeMemory;

static void kInitRuntimeMemory(kRuntimeMemory *memory) {
    memory->size = 4;
    memory->length = 0;
    kRuntimeMemField *temp = (kRuntimeMemField*)malloc(sizeof(kRuntimeMemField) * memory->size);
    if (!temp) {
        const kString errStr = kStrMakeFromC("Couldn't initialize runtime memory");
        kHandleError(kAddErrorT(&errStr, 0, 0, true, ERR_FAIL));
    }
    memory->memory = temp;
}
static void kAddMemoryField(kRuntimeMemory *memory, const kString key, const kVariant value) {
    if (memory->size >= memory->length+1) {
        memory->size = (size_t)(memory->size * 1.5);
        kRuntimeMemField *temp = (kRuntimeMemField*)realloc(memory->memory, sizeof(kRuntimeMemField) * memory->size);
        if (!temp) {
            const kString errStr = kStrMakeFromC("Couldn't resize runtime memory list");
            kHandleError(kAddErrorT(&errStr, 0, 0, true, ERR_FAIL));
        }
        memory->memory = temp;
    }
    memory->memory[memory->length++] = (kRuntimeMemField){.key = key, .value = value};
}

[[nodiscard]]
static size_t kFindMemoryField(const kRuntimeMemory *memory, kString *key) {
    for (size_t i = 0; i < memory->length; i++) {
        if (kStrEquals(&memory->memory[i].key, key))
            return  i;
    }
    return -1;
}

[[nodiscard]]
static kVariant *kGetMemoryValue(const kRuntimeMemory *memory, kString *key) {
    const size_t field = kFindMemoryField(memory, key);
    if (field == (size_t)-1)
        return nullptr;

    return &memory->memory[field].value;
}

#endif //KORD_KMEMORY_H
