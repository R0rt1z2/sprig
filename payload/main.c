#include <bldr.h>
#include <debug.h>
#include <patches.h>

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
    bldr_handshake();
    
    while(1); // handshake should not return
}