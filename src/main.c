#include <stdio.h>
#include <staticDefinitions.h>
#include <kstring.h>
#include <kvariant.h>
#include <kfile.h>
#include <ktoken.h>
#include <kerror.h>
#include <kast.h>

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

    kASTNode varDecl;
    varDecl.nodeType = AST_NODE_VAR_DECL;
    varDecl.kAST_VarDeclStmt.varName = kStrMakeFromC("x");

    kASTNode varName;
    varName.nodeType = AST_NODE_LITERAL;
    kVariant variableNameStr;
    kValSetString(&variableNameStr, "x");
    varName.kAST_LiteralExpr.value = variableNameStr;

    kASTNode numberNode;
    numberNode.nodeType = AST_NODE_LITERAL;
    kVariant numberInt;
    kValSetInt(&numberInt, 4);
    numberNode.kAST_LiteralExpr.value = variableNameStr;


    printf("File:\n%s\nOf size: %ld\n",sourceFile, fileSize);

    kLexer lexer = {
        .source = sourceFile, .cursor = 0, .sourceLength = fileSize,
        .length = 0, .offset = 0
    };

    kErrorInit();
    curFileIdx = kErrorRegisterFile(kGetAbsolutePath(argv[1]).buffer, sourceFile);
    while (!kLexerIsAtEnd(&lexer)) {
        const kToken token = kLexerGetNextToken(&lexer);
        printf("TOKEN %d | %s\n", token.type, token.lexeme.buffer);
    }

    kCheckErrors();

    return 0;
}