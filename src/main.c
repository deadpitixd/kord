#include <stdio.h>
#include <staticDefinitions.h>
#include <kstring.h>
#include <kvariant.h>
#include <kfile.h>
#include <ktoken.h>

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
    const char *sourceFile = kLoadFile("test.krd", &fileSize);
    if (sourceFile) {
        printf("Loaded %s successfully\n", argv[1]);
    }
    else {
        printf("Failed to load %s\n", argv[1]);
    }

    printf("File:\n%s\nOf size: %ld\n",sourceFile, fileSize);

    kLexer lexer = {
        .source = sourceFile, .cursor = 0, .sourceLength = fileSize,
        .length = 0, .offset = 0
    };

    while (!kLexerIsAtEnd(&lexer)) {
        const kToken token = kLexerGetNextToken(&lexer);
        printf("TOKEN %d | %s\n", token.type, token.lexeme.buffer);
    }

    return 0;
}