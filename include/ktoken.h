//
// Created by piti on 11/3/26.
//

#ifndef KORD_KTOKEN_H
#define KORD_KTOKEN_H
#include <ctype.h>
#include <stdint.h>
#include <kstring.h>

typedef enum : uint8_t {
    TOKEN_STRING, // "text"
    TOKEN_DOUBLE, // double literal
    TOKEN_INT, // int literal

    TOKEN_TRUE, // true
    TOKEN_FALSE, // false

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

    TOKEN_ASSIGN, // =
    TOKEN_EQUALS, // ==

    TOKEN_FUNC_DEF, // fn

    TOKEN_SEMICOLON,

    TOKEN_IDENTIFIER,

    TOKEN_EOF
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

// piti - i understand this might not be clean, but it's the best option
[[nodiscard]]
static kTokenType kLexerGetTokenTypeFromStr(const kString *string) {
    if (!strcmp(string->buffer, "int")) return TOKEN_INT_DEF;
    if (!strcmp(string->buffer, "double")) return TOKEN_DOUBLE_DEF;
    if (!strcmp(string->buffer, "float")) return TOKEN_DOUBLE_DEF;
    if (!strcmp(string->buffer, "bool")) return TOKEN_BOOL_DEF;
    if (!strcmp(string->buffer, "string")) return TOKEN_STRING_DEF;
    if (!strcmp(string->buffer, "char")) return TOKEN_CHAR_DEF;
    if (!strcmp(string->buffer, "void")) return TOKEN_VOID_DEF;
    if (!strcmp(string->buffer, "true")) return TOKEN_TRUE;
    if (!strcmp(string->buffer, "false")) return TOKEN_FALSE;
    if (!strcmp(string->buffer, "fn")) return TOKEN_FUNC_DEF;
    if (!strcmp(string->buffer, "+")) return TOKEN_ADD;
    if (!strcmp(string->buffer, "-")) return TOKEN_SUB;
    if (!strcmp(string->buffer, "*")) return TOKEN_MULT;
    if (!strcmp(string->buffer, "/")) return TOKEN_DIV;
    if (!strcmp(string->buffer, "=")) return TOKEN_ASSIGN;
    if (!strcmp(string->buffer, "==")) return TOKEN_EQUALS;
    if (!strcmp(string->buffer, ";")) return TOKEN_SEMICOLON;

    const kIsStrNumericOutput temp = kIsStringNumeric(string);
    if (temp.isNumeric) return temp.isFloatingPoint ? TOKEN_DOUBLE : TOKEN_INT;

    return TOKEN_IDENTIFIER;
}

[[nodiscard]]
static kToken kLexerGetNextToken(kLexer *lexer) {
    // skip whitespace
    while (!kLexerIsAtEnd(lexer) && lexer->source[lexer->cursor] == ' ') {
        lexer->cursor++;
    }

    if (kLexerIsAtEnd(lexer)) {
        return (kToken){ .type = TOKEN_EOF, .offset = lexer->cursor, .length = 0 };
    }

    const size_t startOffset = lexer->cursor;
    char c = lexer->source[lexer->cursor];

    if (c == ';') {
        lexer->cursor++;
        kString tokStr;
        kStrMake(&tokStr);
        kStrAppendC(&tokStr, ';');

        return (kToken){
            .type = TOKEN_SEMICOLON,
            .lexeme = tokStr,
            .length = 1,
            .offset = startOffset
        };
    }

    kString tokStr;
    kStrMake(&tokStr);

    while (!kLexerIsAtEnd(lexer)) {
        c = lexer->source[lexer->cursor];
        if (c == ' ' || c == ';') {
            break;
        }
        kStrAppendC(&tokStr, c);
        lexer->cursor++;
    }

    return (kToken) {
        .type = kLexerGetTokenTypeFromStr(&tokStr),
        .lexeme = tokStr,
        .length = lexer->cursor - startOffset,
        .offset = startOffset
    };
}

#endif //KORD_KTOKEN_H
