//
// Created by pitiv on 11.11.2026.
//

#ifndef KORD_KPARSE_H
#define KORD_KPARSE_H

#include <ktoken.h>
#include <kerror.h>
#include <kast.h>

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

static void kAddToken(kParser *parser, const kToken *token) {
    if (parser->tokenSize <= parser->tokenCount+1) {
        parser->tokenSize = (size_t)(parser->tokenSize * 1.5);
        kToken *temp = realloc(parser->tokens, sizeof(kToken) * parser->tokenSize);
        if (!temp) {
            const kString msg = kStrMakeFromC("Failed to reallocate and add an element to the parser");
            kHandleError(kAddError(&msg, 0, 0, true));
        }
        parser->tokens = temp;
    }
    parser->tokens[parser->tokenCount++] = *token;
}

[[nodiscard]]
static bool kParserIsAtEnd(const kParser *parser) {
    return parser->cursor >= parser->tokenCount;
}

[[nodiscard]]
static kToken *kParserPeek(const kParser *parser) {
    return &parser->tokens[parser->cursor];
}

[[nodiscard]]
static kToken *kParserPrevious(const kParser *parser) {
    return &parser->tokens[parser->cursor - 1];
}

static kToken *kParserAdvance(kParser *parser) {
    if (!kParserIsAtEnd(parser)) {
        parser->cursor++;
    }
    return kParserPrevious(parser);
}

[[nodiscard]]
static bool kParserCheck(const kParser *parser, const kTokenType type) {
    if (kParserIsAtEnd(parser)) return false;
    return kParserPeek(parser)->type == type;
}

static kToken *kParserExpect(kParser *parser, const kTokenType type) {
    if (kParserIsAtEnd(parser)) {
        const kString msg = kStrMakeFromC("Unexpected end of file while expecting token");
        kAddError(&msg, 0, 0, true);
        return nullptr;
    }

    const kToken *token = kParserPeek(parser);

    if (token->type != type) {
        kString str = kStrMakeFromC("Expected token type ");
        char number[64];

        snprintf(number, sizeof(number), "%d", type);
        kStrAppend(&str, number);
        kStrAppend(&str, " but got type ");

        snprintf(number, sizeof(number), "%d", token->type);
        kStrAppend(&str, number);

        kAddErrorT(&str, token->offset, token->length, true, ERR_PARSE_REQ_TOKEN);
        return nullptr;
    }

    return kParserAdvance(parser);
}

[[nodiscard]]
static kASTNode *kParsePrimary(kParser *parser) {
    if (kParserIsAtEnd(parser) || kParserCheck(parser, TOKEN_SEMICOLON)) {
        return nullptr;
    }

    const kToken *tok = kParserPeek(parser);

    if (tok->type == TOKEN_STRING || tok->type == TOKEN_INT || tok->type == TOKEN_DOUBLE) {
        kASTNode *node = (kASTNode*)malloc(sizeof(kASTNode));
        if (node == nullptr) return nullptr;
        memset(node, 0, sizeof(kASTNode));

        node->nodeType = AST_NODE_LITERAL;
        kVariant value;

        if (tok->type == TOKEN_STRING) {
            kValSetString(&value, tok->lexeme.buffer);
        } else if (tok->type == TOKEN_INT) {
            kValSetInt(&value, atoi(tok->lexeme.buffer));
        } else if (tok->type == TOKEN_DOUBLE) {
            kValSetDouble(&value, atof(tok->lexeme.buffer));
        }

        node->kAST_LiteralExpr.value = value;
        kParserAdvance(parser);
        return node;
    }

    return nullptr;
}

[[nodiscard]]
static kASTNode *kParseExpression(kParser *parser) {
    kASTNode *left = kParsePrimary(parser);
    if (left == nullptr) return nullptr;

    while (!kParserIsAtEnd(parser) && kParserCheck(parser, TOKEN_ADD) || kParserCheck(parser, TOKEN_SUB) ||
            kParserCheck(parser, TOKEN_MULT) || kParserCheck(parser, TOKEN_DIV)) {
        kBinaryOperator operator;

        switch (kParserPeek(parser)->type) {
            case TOKEN_ADD:
                operator = BINOP_ADD;
                break;
            case TOKEN_SUB:
                operator = BINOP_SUB;
                break;
            case TOKEN_MULT:
                operator = BINOP_MULT;
                break;
            case TOKEN_DIV:
                operator = BINOP_DIV;
                break;
            default:
                operator = BINOP_ADD;
                break;
        }

        kParserAdvance(parser);

        kASTNode *right = kParsePrimary(parser);
        if (right == nullptr) {
            return left;
        }

        kASTNode *binOpNode = (kASTNode*)malloc(sizeof(kASTNode));
        if (binOpNode == nullptr) return left;
        memset(binOpNode, 0, sizeof(kASTNode));

        binOpNode->nodeType = AST_NODE_BINOP;
        binOpNode->kAST_BinaryExpr.left = left;
        binOpNode->kAST_BinaryExpr.op = operator;
        binOpNode->kAST_BinaryExpr.right = right;

        left = binOpNode;
    }

    return left;
}

[[nodiscard]]
static kASTNode *kParseTokens(kParser *parser) {
    kASTNode *head = nullptr;
    kASTNode *tail = nullptr;
    bool isConst = false;

    while (!kParserIsAtEnd(parser)) {
        if (kParserCheck(parser, TOKEN_CONST)) {
            if (isConst) {
                kString errStr = kStrMakeFromC("Cannot use two const definitions");
                kAddErrorT(&errStr, kParserPeek(parser)->offset, kParserPeek(parser)->length, true, ERR_FAIL);
            }
            if (parser->cursor + 1 >= parser->tokenCount || parser->tokens[parser->cursor + 1].type != TOKEN_I32) {
                kString errStr = kStrMakeFromC("const must be placed before variable or function definition");
                kAddErrorT(&errStr, kParserPeek(parser)->offset, kParserPeek(parser)->length, true, ERR_FAIL);
            }
            isConst = true;
            kParserAdvance(parser);
            continue;
        }

        if (kParserCheck(parser, TOKEN_I32) && (parser->cursor + 1 < parser->tokenCount) && parser->tokens[parser->cursor + 1].type == TOKEN_IDENTIFIER) {
            kASTNode *varDeclNode = (kASTNode*)malloc(sizeof(kASTNode));

            if (varDeclNode == nullptr) return nullptr;
            memset(varDeclNode, 0, sizeof(kASTNode));
            varDeclNode->nodeType = AST_NODE_VAR_DECL;
            varDeclNode->kAST_VarDeclStmt.isConst = isConst;
            isConst = false;

            switch (kParserAdvance(parser)->type) {
                case TOKEN_I32:
                    varDeclNode->kAST_VarDeclStmt.valueType = TYPE_INT;
                    break;
                case TOKEN_DOUBLE:
                    varDeclNode->kAST_VarDeclStmt.valueType = TYPE_DOUBLE;
                    break;
                case TOKEN_STRING:
                    varDeclNode->kAST_VarDeclStmt.valueType = TYPE_STRING;
                    break;
                default:
                    varDeclNode->kAST_VarDeclStmt.valueType = TYPE_VOID;
                    break;
            }

            varDeclNode->kAST_VarDeclStmt.varName = kParserAdvance(parser)->lexeme;

            if (kParserAdvance(parser)->type == TOKEN_ASSIGN) {
                varDeclNode->kAST_VarDeclStmt.defaultValue = kParseExpression(parser);
            } else {
                varDeclNode->kAST_VarDeclStmt.defaultValue = nullptr;
            }

            kParserExpect(parser, TOKEN_SEMICOLON);

            if (head == nullptr) {
                head = varDeclNode;
                tail = head;
            } else {
                tail->next = varDeclNode;
                tail = varDeclNode;
            }
        } else {
            kParserAdvance(parser);
        }
    }
    return head;
}

#endif //KORD_KPARSE_H
