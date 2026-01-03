#include <stdint.h>
#include <stddef.h>

#include <debug.h>
#include <patches.h>

struct bldr_command_handler {
    void *priv;
    uint32_t attr;
    void *cb;
};

void main(void) {
    printf("\n");
    printf("           .--._.--.          \n");
    printf("          ( O     O )         \n");
    printf("          /   . .   \\         \n");
    printf("         .`._______.'.        \n");
    printf("        /(           )\\       \n");
    printf("      _/  \\  \\   /  /  \\_     \n");
    printf("   .~   `  \\  \\ /  /  '   ~.  \n");
    printf("  {    -.   \\  V  /   .-    } \n");
    printf("_ _`.    \\  |  |  |  /    .'_ _\n");
    printf(">_       _} |  |  | {_       _<\n");
    printf(" /. - ~ ,_-'  .^.  `-_, ~ - .\\ \n");
    printf("         '-'|/   \\|`-`        \n");
    printf("\n   Hello world from payload \n\n");
    
    patch_apply_all();
    
    struct bldr_command_handler handler = {
        .priv = NULL,
        .attr = 0,
        .cb = (void *)0x02048818
    };
    
    printf("\nAbout to handshake...\n\n");

    ((int (*)(struct bldr_command_handler *))(0x02048404))(&handler);
    
    while(1);
}