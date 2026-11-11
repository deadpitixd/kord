//
// Created by pitiv on 11.11.2026.
//

#ifndef KORD_KPARSE_H
#define KORD_KPARSE_H

#include <ktoken.h>
#include <kerror.h>

typedef struct {
    kToken *tokens;
    size_t tokenCount;
    size_t tokenSize;
    size_t cursor;
} kParser;

static void kInitParser(kParser *parser) {
    parser->tokenSize = 1;
    parser->tokenCount = 0;
    parser->cursor = 0;
    kToken *temp = (kToken*)malloc(sizeof(kToken));
    if (!temp) {
        const kString msg = kStrMakeFromC("Failed to allocate the parser");
        kHandleError(kAddError(&msg, 0, 0, true));
    }
    parser->tokens = temp;
}

static void kAddToken(kToken *token) {

}

#endif //KORD_KPARSE_H
