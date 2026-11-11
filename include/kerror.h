//
// Created by piti on 11/9/26.
//

#ifndef KORD_KERROR_H
#define KORD_KERROR_H

#include <kstring.h>

#include <stdlib.h>
#include <stdint.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOSERVICE
#define NOMCX
#define NOIME
#define NOHELP
#include <windows.h>
#else
#include <limits.h>
#endif


static kString kGetAbsolutePath(const char *path) {
    char resolved[
#ifdef  _WIN32
    MAX_PATH
#else
    PATH_MAX
#endif
        ];
    kString kResolved;
    kStrMake(&kResolved);
#ifdef _WIN32
    if (_fullpath(resolved, path, MAX_PATH) != NULL) {
        kStrSet(&kResolved, resolved);
        return kResolved;
    }
#else
    if (realpath(path, resolved) != NULL) {
        kStrSet(&kResolved, resolved);
        return kResolved;
    }
#endif
    // fallback to original path if this fails
    kStrSet(&kResolved, path);
    return kResolved;
}

typedef enum kErrorType_e : uint8_t {
    ERR_OK,

    ERR_WARNING,

    ERR_UNKNOWN,

    ERR_LEX_UNK_CHAR,
    ERR_LEX_UNTERMINATED,
    ERR_LEX_INVALID_NUM,
    ERR_LEX_INVALID,

    ERR_PARSE_REQ_TOKEN,
    ERR_PARSE_INVALID_EXPR,
    ERR_PARSE_INVALID_ARR_SIZE,

    ERR_SEMANTIC_UNDEF_SYMBOL,
    ERR_SEMANTIC_REDEF,
    ERR_SEMANTIC_TYPE_MISMATCH,
    ERR_SEMANTIC_INVALID_OP,

    ERR_FAIL,

    ERR_END
} kErrorType;

typedef struct {
    kString path;
    kString content;
} kSourceFile;

static kSourceFile *fileTable = nullptr;
static size_t fileCount = 0;
static size_t fileCapacity = 0;

static size_t curFileIdx = 0;

// register a file and return id
static size_t kErrorRegisterFile(const char *path, const char *content) {
    if (fileCount >= fileCapacity) {
        fileCapacity = fileCapacity == 0 ? 4 : fileCapacity * 2;
        fileTable = (kSourceFile*)realloc(fileTable, fileCapacity * sizeof(kSourceFile));
    }
    kString kPath;
    kStrMake(&kPath);
    kStrSet(&kPath, path);

    kString kContent;
    kStrMake(&kContent);
    kStrSet(&kContent, content);
    fileTable[fileCount] = (kSourceFile){
        .path = kPath,
        .content = kContent
    };
    return fileCount++;
}

[[nodiscard]]
static size_t kErrorFindFile(const char *path) {
    for (size_t i = 0; i < fileCount; i++) {
        if (kStrEqualsC(&fileTable[i].path, path)) {
            return i;
        }
    }
    return SIZE_MAX;
}

static void kErrorSetCurFile(const char *path) {
    curFileIdx = kErrorFindFile(path);
}

typedef struct kError_s  {
    kString msg;
    size_t offset;
    size_t length;
    kErrorType errType;
    bool forceExit; // will the program compile / run
    size_t fileIndex; // index in the file list
} kError;

static kError *errorList;
static size_t errorListSize;
static size_t errorListLength;

static void kErrorInit() {
    errorList = (kError*)malloc(sizeof(kError));
    errorListSize = 1;
    errorListLength = 0;
}

static kError *kAddErrorT(const kString *msg, const size_t offset, const size_t length, const bool forceExit, const kErrorType error) {
    if (errorListSize <= errorListLength + 1) {
        errorListSize = (size_t)((float)errorListSize * 1.5);
        kError *temp = (kError*)realloc(errorList, errorListSize * sizeof(kError));
        if (!temp) {
            printf("Failed to add element of error %d with message %s, couldn't resize error list", error, msg->buffer);
            return nullptr;
        }
        errorList = temp;
    }
    errorList[errorListLength++] = (kError){.length = length, .offset = offset, .forceExit = forceExit, .errType = error, .msg = kStrDup(msg) };
    return &errorList[errorListLength];
}

static kError *kAddError(const kString *msg, const size_t offset, const size_t length, const bool forceExit) {
    return kAddErrorT(msg, offset, length, forceExit, ERR_UNKNOWN);
}

static void kPrintError(const kError *err, const kSourceFile *file, const char *bgcolor);

bool exitAfterCheck = false;

static void kHandleError(const kError *error) {
    kPrintError(error, &fileTable[error->fileIndex], error->errType == ERR_WARNING ?
                                    kC_bg_yellow : kC_bg_magenta);
    if (error->forceExit) exitAfterCheck = true;
}

static void kCheckErrors() {
    for (size_t i = 0; i < errorListLength; i++) {
        kHandleError(&errorList[i]);
    }
    if (exitAfterCheck) {
        exit(-1);
    }
}

static void kPrintError(const kError *err, const kSourceFile *file, const char *bgcolor) {
    const char *source = file->content.buffer;
    const char *filename = file->path.buffer;

    size_t current_offset = 0;
    size_t line_num = 1;
    size_t line_start = 0;

    while (current_offset < err->offset && source[current_offset] != '\0') {
        if (source[current_offset] == '\n') {
            line_num++;
            line_start = current_offset + 1;
        }
        current_offset++;
    }

    size_t line_end = err->offset;
    while (source[line_end] != '\0' && source[line_end] != '\n') {
        line_end++;
    }

    size_t col_num = (err->offset - line_start) + 1;

    int margin_width = snprintf(nullptr, 0, "%zu", line_num);

    char squiggle[256];
    if (err->length > 1) {
        const size_t s_len = err->length < sizeof(squiggle) - 1 ? err->length : sizeof(squiggle) - 1;
        squiggle[0] = '^';
        for (size_t i = 1; i < s_len - 1; i++) {
            squiggle[i] = '~';
        }
        squiggle[s_len - 1] = '^';

        squiggle[s_len] = '\0';
    } else {
        squiggle[0] = '^';
        squiggle[1] = '\0';
    }

    char padding[32];
    if (margin_width > 0 && margin_width < (int)sizeof(padding)) {
        for (int i = 0; i < margin_width; i++) padding[i] = ' ';
        padding[margin_width] = '\0';
    } else {
        padding[0] = '\0';
    }

    printf("  --> %s:%zu:%zu\n", filename, line_num, col_num);
    printf("  %s |\n", padding);
    printf("  %zu | %s%.*s%s\n", line_num, kC_b_magenta, (int)(line_end - line_start), source + line_start, kC_reset);

    char col_padding[256];
    size_t pad_len = col_num > 1 ? col_num - 1 : 0;
    if (pad_len >= sizeof(col_padding)) pad_len = sizeof(col_padding) - 1;
    for (size_t i = 0; i < pad_len; i++) col_padding[i] = ' ';
    col_padding[pad_len] = '\0';

    printf("  %s | %s%s%s%s\n", padding, col_padding, kC_bg_magenta, squiggle, kC_reset);
    printf("%s%s%s\n", bgcolor ? bgcolor : kC_bg_red, err->msg.buffer, kC_reset);
}

#endif //KORD_KERROR_H
