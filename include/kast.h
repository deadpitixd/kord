//
// Created by piti on 11/10/26.
//

#ifndef KORD_KAST_H
#define KORD_KAST_H

#include <stdint.h>
#include <kvariant.h>

typedef enum : uint8_t{
    AST_NODE_LITERAL,
    AST_NODE_VAR_DECL,
    AST_NODE_BINOP,
    AST_NODE_ASSIGN_STMT,
    AST_NODE_PRINT_STMT,
    AST_NODE_RETURN_STMT,
    AST_NODE_BLOCK
} kASTNodeType;

typedef enum : uint8_t{
    BINOP_ADD,
    BINOP_SUB,
    BINOP_MULT,
    BINOP_DIV
} kBinaryOperator;

typedef struct kASTNode_s {
    kASTNodeType nodeType;
    struct kASTNode_s *next;
    bool isExpr;
    // stuff for error handling
    size_t offset;
    size_t length;
    union {
        struct {
            kVariant value;
        } kAST_LiteralExpr;
        struct {
            struct kASTNode_s *left;
            kBinaryOperator op;
            struct kASTNode_s *right;
        } kAST_BinaryExpr;
        struct {
            bool isConst;
            kString varName;
            kVariant defaultValue;
        } kAST_VarDeclStmt;
        struct {
            struct kASTNode_s *assignValue;
            kString varName;
        } kAST_AssignStmt;
    };
} kASTNode;

#endif //KORD_KAST_H
