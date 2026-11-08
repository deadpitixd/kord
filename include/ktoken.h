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

    TOKEN_RIGHT_ARROW, // ->
    TOKEN_LEFT_ARROW, // <-

    TOKEN_LBRACE, // {
    TOKEN_RBRACE, // )

    TOKEN_LPAREN, // (
    TOKEN_RPAREN, // )

    TOKEN_GREATER, // >
    TOKEN_LESS, // <

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

    bool isText = false;
    while (!kLexerIsAtEnd(lexer)) {
        c = lexer->source[lexer->cursor];
        isText = isalpha(c) || isText;
        if (c == ' ' || c == ';') {
            break;
        }
        if (isText && !isalpha(c)) {
            break;
        }
        kStrAppendC(&tokStr, c);
        lexer->cursor++;
        if (!isalpha(c)) return kLexerHandleJoining(c, lexer, &tokStr, startOffset);
    }

    return (kToken) {
        .type = kLexerGetTokenTypeFromStr(&tokStr),
        .lexeme = tokStr,
        .length = lexer->cursor - startOffset,
        .offset = startOffset
    };
}

#endif //KORD_KTOKEN_H
