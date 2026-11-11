#include <stdio.h>
#include <staticDefinitions.h>
#include <kstring.h>
#include <kvariant.h>
#include <kfile.h>
#include <ktoken.h>
#include <kerror.h>
#include <kast.h>
#include <kmemory.h>
#include <kparse.h>

int main(const int argc, char** argv) {
    if (argc == 1) {
        printf(
            "Kord - v" KORD_VERSION_STRING "\n"
                    "LGPLv2 License, Made by %s@piti%s on GitHub\n"
                    "Available on deadpitixd/kord\n"
                    "Use %s--help%s for more information\n", kC_b_green, kC_reset, kC_b_red, kC_reset);
        return 0;
    }
    if (!strcmp(argv[1], "--help")) {
        printf(KORD_HELP);
        return 0;
    }
    size_t fileSize = 0;
    const char *sourceFile = kLoadFile(argv[1], &fileSize);

    if (sourceFile) {
        printf("Loaded %s successfully at path %s\n", argv[1], kGetAbsolutePath(argv[1]).buffer);
    }
    else {
        printf("Failed to load %s\n", argv[1]);
    }

    printf("File:\n%s\nOf size: %ld\n",sourceFile, fileSize);

    kLexer lexer = {
        .source = sourceFile, .cursor = 0, .sourceLength = fileSize,
        .length = 0, .offset = 0
    };

    kParser parser;
    kInitParser(&parser);

    kErrorInit();
    curFileIdx = kErrorRegisterFile(kGetAbsolutePath(argv[1]).buffer, sourceFile);
    while (!kLexerIsAtEnd(&lexer)) {
        const kToken token = kLexerGetNextToken(&lexer);
        kAddToken(&parser, &token);

        printf("TOKEN %d | %s\n", token.type, token.lexeme.buffer);
    }

    kCheckErrors();
    printf("Parser got %ld tokens\n", parser.tokenCount);

    kRuntimeMemory mem;
    kInitRuntimeMemory(&mem);

    evaluationMemory = &mem;

    kEvaluateNode(kParseTokens(&parser));

    kString varNameStr = kStrMakeFromC("x");

    printf("Got number %f at field 'x' in memory\n", kValGetDouble(kGetMemoryValue(&mem, &varNameStr)));

    return 0;
}

/*
   // root / variable declaration
    kASTNode varDecl = {0};
    varDecl.nodeType = AST_NODE_VAR_DECL;
    varDecl.kAST_VarDeclStmt.varName = str;


    // number node
    kASTNode numberNode = {0};
    numberNode.nodeType = AST_NODE_LITERAL;
    kVariant numberInt;
    kValSetInt(&numberInt, 5);
    numberNode.kAST_LiteralExpr.value = numberInt;

    // binary operation
    kASTNode binaryOp = {0};
    binaryOp.nodeType = AST_NODE_BINOP;
    binaryOp.kAST_BinaryExpr.left = &numberNode;
    binaryOp.kAST_BinaryExpr.right = &numberNode;
    binaryOp.kAST_BinaryExpr.op = BINOP_ADD;

    varDecl.kAST_VarDeclStmt.defaultValue = &numberNode;


    // assign stmt
    kASTNode assignNode = {0};
    assignNode.nodeType = AST_NODE_ASSIGN_STMT;
    assignNode.kAST_AssignStmt.assignValue = &binaryOp;
    assignNode.kAST_AssignStmt.varName = str;

    varDecl.next = &assignNode;

    kRuntimeMemory mem;
    kInitRuntimeMemory(&mem);

    evaluationMemory = &mem;
    kEvaluateNode(&varDecl);


    printf("Got number %d at field 'x' in memory\n", kValGetInt(kGetMemoryValue(&mem, &str)));

 */