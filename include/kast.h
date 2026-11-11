//
// Created by piti on 11/10/26.
//

#ifndef KORD_KAST_H
#define KORD_KAST_H

#include <stdint.h>
#include <kvariant.h>
#include <ktoken.h>
#include <kmemory.h>

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
            struct kASTNode_s *defaultValue;
        } kAST_VarDeclStmt;
        struct {
            struct kASTNode_s *assignValue;
            kString varName;
        } kAST_AssignStmt;
    };
} kASTNode;

// these are given by the user
static kASTNode *outputNodes;
static void kCompileToAST(kToken *token, const size_t length) {

}

static kRuntimeMemory *evaluationMemory;
static kVariant kEvaluateNode(kASTNode *node) {
    switch (node->nodeType) {
        case AST_NODE_VAR_DECL:
            kASTNode *valueNode = node->kAST_VarDeclStmt.defaultValue;
            kAddMemoryField(evaluationMemory, &node->kAST_VarDeclStmt.varName, kEvaluateNode(valueNode));
            break;
        case AST_NODE_LITERAL:
            return node->kAST_LiteralExpr.value;
        case AST_NODE_BINOP:
            const kBinaryOperator *op = &node->kAST_BinaryExpr.op;
            kVariant a = kEvaluateNode(node->kAST_BinaryExpr.left);
            kVariant b = kEvaluateNode(node->kAST_BinaryExpr.right);
            if (a.type != b.type) {
                return kDefaultValueOfType(a.type);
            }
            kVariant outputValue;
            switch (a.type) {
                case TYPE_INT:
                    kValSetInt(&outputValue, kValGetInt(&a) + kValGetInt(&b));
                    break;
                case TYPE_DOUBLE:
                    kValSetDouble(&outputValue, kValGetDouble(&a) + kValGetDouble(&b));
                    break;
                case TYPE_STRING:
                    kValSetString(&outputValue, "");
                    break;
            }


            return outputValue;

        case AST_NODE_ASSIGN_STMT:
            const size_t idx = kFindMemoryField(evaluationMemory, &node->kAST_AssignStmt.varName);
            kVariant value = kEvaluateNode(node->kAST_AssignStmt.assignValue);
            if (value.type == evaluationMemory->types[idx]) {
                evaluationMemory->memory[idx].value = value;
            }

            break;
        default:
            break;
    }
    if (node->next == nullptr) {
        return (kVariant){};
    }
    return kEvaluateNode(node->next);
}

#endif //KORD_KAST_H
