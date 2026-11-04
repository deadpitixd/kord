//
// Created by piti on 11/4/26.
//

#ifndef KORD_KFILE_H
#define KORD_KFILE_H

#include <stdio.h>
#include <stdlib.h>

// piti - loads a file into a string
static char* kLoadFile(const char *filename, size_t *outSize) {
    FILE *file = fopen(filename, "rb");
    if (!file) return nullptr;

    // gets the file size
    fseek(file, 0, SEEK_END);
    const long length = ftell(file);
    rewind(file);

    if (length < 0) {
        fclose(file);
        return nullptr;
    }

    // allocates buffer
    char *buffer = (char*)malloc(length + 1);
    if (!buffer) {
        fclose(file);
        return nullptr;
    }

    const size_t readBytes = fread(buffer, 1, length, file);
    buffer[readBytes] = '\0';
    fclose(file);

    if (outSize) *outSize = readBytes;
    return buffer;
}

#endif //KORD_KFILE_H
