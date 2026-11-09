//
// Created by piti on 11/3/26.
//

#ifndef KORD_KTOKEN_H
#define KORD_KTOKEN_H
#include <ctype.h>
#include <stdint.h>
#include <kstring.h>

#include "kerror.h"

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

    TOKEN_RIGHT_ARROW, // ->
    TOKEN_LEFT_ARROW, // <-

    TOKEN_LBRACE, // {
    TOKEN_RBRACE, // )

    TOKEN_LPAREN, // (
    TOKEN_RPAREN, // )

    TOKEN_GREATER, // >
    TOKEN_LESS, // <

    TOKEN_I8,
    TOKEN_I16,
    TOKEN_I32,
    TOKEN_I64,

    TOKEN_UI8,
    TOKEN_UI16,
    TOKEN_UI32,
    TOKEN_UI64,

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
    if (!strcmp(string->buffer, "->")) return TOKEN_RIGHT_ARROW;
    if (!strcmp(string->buffer, "<-")) return TOKEN_LEFT_ARROW;
    if (!strcmp(string->buffer, "{")) return TOKEN_LBRACE;
    if (!strcmp(string->buffer, "}")) return TOKEN_RBRACE;
    if (!strcmp(string->buffer, "(")) return TOKEN_LPAREN;
    if (!strcmp(string->buffer, ")")) return TOKEN_RPAREN;
    if (!strcmp(string->buffer, ">")) return TOKEN_GREATER;
    if (!strcmp(string->buffer, "<")) return TOKEN_LESS;

    const kIsStrNumericOutput temp = kIsStringNumeric(string);
    if (temp.isNumeric) return temp.isFloatingPoint ? TOKEN_DOUBLE : TOKEN_INT;

    return TOKEN_IDENTIFIER;
}

// piti - joins tokens into symbols, example:
// '-' + '>' = '->'
// ONLY USED BY THE LEXER
[[nodiscard]]
static kToken kLexerHandleJoining(const char c, kLexer *lexer, kString *string, const size_t offset) {
    switch (c) {
        case '-': {
            if (kLexerPeek(lexer) == '>') {
                kLexerAdvance(lexer);
                kStrAppendC(string, '>');
                return (kToken) { .type = TOKEN_RIGHT_ARROW, .offset = offset, .length = lexer->cursor - offset, .lexeme = *string
                };
            }
            break;
        }
        case '<': {
            if (kLexerPeek(lexer) == '-') {
                kLexerAdvance(lexer);
                kStrAppendC(string, '-');
                return (kToken) {.type = TOKEN_LEFT_ARROW,.offset = offset,.length = lexer->cursor - offset,.lexeme = *string
                };
            }
            break;
        }
        case '=': {
            if (kLexerPeek(lexer) == '=') {
                kLexerAdvance(lexer);
                kStrAppendC(string, '=');
                return (kToken) {.type = TOKEN_EQUALS, .offset = offset, .length = lexer->cursor - offset, .lexeme = *string
                };
            }
            break;
        }
        default:
            break;
    }

    return (kToken) {
        .type = kLexerGetTokenTypeFromStr(string),
        .lexeme = *string,
        .length = lexer->cursor - offset,
        .offset = offset
    };
}
[[nodiscard]]
static kToken kLexerGetNextToken(kLexer *lexer) {
    // skip whitespace
    while (!kLexerIsAtEnd(lexer) && isspace((unsigned char)lexer->source[lexer->cursor])) {
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

    // strings
    if (c == '"') {
        lexer->cursor++;

        while (!kLexerIsAtEnd(lexer) && lexer->source[lexer->cursor] != '"') {
            kStrAppendC(&tokStr, lexer->source[lexer->cursor]);
            lexer->cursor++;
        }

        if (!kLexerIsAtEnd(lexer)) {
            lexer->cursor++; // skip closing quote
        }

        return (kToken){
            .type = TOKEN_STRING,
            .lexeme = tokStr,
            .length = lexer->cursor - startOffset,
            .offset = startOffset
        };
    }

    bool isTypeUnsigned = false;
    if (c == 'u') {
        isTypeUnsigned = true;
        lexer->cursor++;
    }

    // types
    if (kLexerPeek(lexer) == 'i') {
        kLexerAdvance(lexer);
        if (isTypeUnsigned) kStrAppendC(&tokStr, 'u');
        kStrAppendC(&tokStr, 'i');
        while (!kLexerIsAtEnd(lexer) && isdigit(lexer->source[lexer->cursor])) {
            kStrAppendC(&tokStr, lexer->source[lexer->cursor]);
            lexer->cursor++;
        }
        kTokenType outputType = TOKEN_EOF;
        if (!isTypeUnsigned) {
            if (!strcmp(tokStr.buffer, "i8")) outputType = TOKEN_I8;
            if (!strcmp(tokStr.buffer, "i16")) outputType = TOKEN_I16;
            if (!strcmp(tokStr.buffer, "i32")) outputType = TOKEN_I32;
            if (!strcmp(tokStr.buffer, "i64")) outputType = TOKEN_I64;
        }else {
            if (!strcmp(tokStr.buffer, "ui8")) outputType = TOKEN_UI8;
            if (!strcmp(tokStr.buffer, "ui16")) outputType = TOKEN_UI16;
            if (!strcmp(tokStr.buffer, "ui32")) outputType = TOKEN_UI32;
            if (!strcmp(tokStr.buffer, "ui64")) outputType = TOKEN_UI64;
        }
        isTypeUnsigned = false;

        if (outputType == TOKEN_EOF) {
            kString error;
            kStrMake(&error);
            kStrSet(&error, "Invalid size of int given");
            kAddErrorT(&error, startOffset, tokStr.length, true, ERR_LEX_INVALID_NUM);
        }

        return (kToken){.type = outputType, .length = lexer->length - startOffset, .lexeme = tokStr, .offset = startOffset };
    }

    // char
    // piti - chars are macros for ints
    if (kLexerPeek(lexer) == 'c') {
        kStrAppendC(&tokStr, 'i');
        lexer->cursor++;
        while (!kLexerIsAtEnd(lexer) && isdigit(lexer->source[lexer->cursor])) {
            kStrAppendC(&tokStr, lexer->source[lexer->cursor]);
            lexer->cursor++;
        }
        kTokenType outputType = TOKEN_EOF;

        if (isTypeUnsigned) {
            const kString errorMessage = kStrMakeFromC("Character types cannot be unsigned");
            kAddErrorT(&errorMessage, startOffset, tokStr.length+1, true, ERR_LEX_INVALID);
        }

        if (!strcmp(tokStr.buffer, "i8")) outputType = TOKEN_I8;
        if (!strcmp(tokStr.buffer, "i16")) outputType = TOKEN_I16;
        if (!strcmp(tokStr.buffer, "i32")) outputType = TOKEN_I32;
        if (!strcmp(tokStr.buffer, "i64")) outputType = TOKEN_I64;

        if (outputType == TOKEN_EOF) {
            kString error;
            kStrMake(&error);
            kStrSet(&error, "Invalid size of char given");
            kAddErrorT(&error, startOffset, tokStr.length, true, ERR_LEX_INVALID_NUM);
        }

        return (kToken){.type = outputType, .length = lexer->length - startOffset, .lexeme = tokStr, .offset = startOffset };
    }

    // resets the cursor to the position before
    if (isTypeUnsigned) lexer->cursor--;

    if (isalpha(c) || c == '_') {
        while (!kLexerIsAtEnd(lexer)) {
            const char cur = lexer->source[lexer->cursor];
            // allow lets, digits and underscores
            if (isalnum((unsigned char)cur) || cur == '_') {
                kStrAppendC(&tokStr, cur);
                lexer->cursor++;
            } else {
                break;
            }
        }

        return (kToken) {
            .type = kLexerGetTokenTypeFromStr(&tokStr),
            .lexeme = tokStr,
            .length = lexer->cursor - startOffset,
            .offset = startOffset
        };
    }

    while (!kLexerIsAtEnd(lexer)) {
        c = lexer->source[lexer->cursor];
        if (c == ' ' || c == ';') {
            break;
        }
        kStrAppendC(&tokStr, c);
        lexer->cursor++;
        if (!isalpha(c) && !isdigit(c)) return kLexerHandleJoining(c, lexer, &tokStr, startOffset);
    }

    return (kToken) {
        .type = kLexerGetTokenTypeFromStr(&tokStr),
        .lexeme = tokStr,
        .length = lexer->cursor - startOffset,
        .offset = startOffset
    };
}

#endif //KORD_KTOKEN_H
