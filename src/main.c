#include <stdio.h>
#include <staticDefinitions.h>
#include <kstring.h>
#include <kvariant.h>

int main(const int argc, char** argv) {
    if (argc == 1)
        printf(
            "Kord - v" KORD_VERSION_STRING "\n"
                    "LGPLv2 License, Made by @piti on GitHub\n"
                    "Available on deadpitixd/kord\n"
                    "Use --help for more information\n");

    kVariant var;
    kValSetString(&var, "Hello");
    kStrAppend(kValGetString(&var), " World!");
    printf("Kstring: %s\n",kValGetString(&var)->buffer);

    return 0;
}