//
// Created by piti on 11/3/26.
//

#ifndef KORD_KTOKEN_H
#define KORD_KTOKEN_H
#include <stdint.h>
#include <kstring.h>

typedef enum : uint8_t {
    TOKEN_STRING, // "text"
    TOKEN_DOUBLE, // double literal
    TOKEN_INT, // int literal

    TOKEN_INT_DEF, // int
    TOKEN_DOUBLE_DEF, // double / float
    TOKEN_BOOL_DEF, // bool
    TOKEN_STRING_DEF, // string
    TOKEN_CHAR_DEF, // char
    TOKEN_VOID_DEF, // void

    TOKEN_ADD, // +
    TOKEN_SUB, // -
    TOKEN_MULT, // *
    TOKEN_DIV, // /

    TOKEN_FUNC_DEF // fn
} kTokenType;

typedef struct {
    // token type
    kTokenType type;
    // value
    kString lexeme;
    // used for error reporting 
    size_t offset;
    size_t length;
} kToken;

typedef struct {
    const char *source;  // file contents
    size_t cursor;       // cursor pos in source
    size_t sourceLength; // length of source
    // error handling
    size_t length;       // source code length
    size_t offset;       // current line
} kLexer;

[[nodiscard]]
static bool kLexerIsAtEnd(const kLexer *lexer) {
    return lexer->cursor >= lexer->sourceLength;
}

[[nodiscard]]
static char kLexerPeek(const kLexer *lexer) {
    if (kLexerIsAtEnd(lexer)) return '\0';
    return lexer->source[lexer->cursor];
}

static char kLexerAdvance(kLexer *lexer) {
    if (kLexerIsAtEnd(lexer)) return '\0';
    return lexer->source[lexer->cursor++];
}

[[nodiscard]]
static kToken kLexerGetNextToken(kLexer* lexer) {

}

#endif //KORD_KTOKEN_H
