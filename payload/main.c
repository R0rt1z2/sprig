#include <bldr.h>
#include <debug.h>
#include <hooks.h>
#include <patches.h>

void main(void) {
    printf("\n");
    printf("           .--._.--.          \n");
    printf("          ( O     O )         \n");
    printf("          /   . .   \\         \n");
    printf("         .`._______.'.        \n");
    printf("        /(           )\\                              _       \n");
    printf("      _/  \\  \\   /  /  \\_           ___ _ __  _ __(_) __ _ \n");
    printf("   .~   `  \\  \\ /  /  '   ~.       / __| '_ \\| '__| |/ _` |\n");
    printf("  {    -.   \\  V  /   .-    }      \\__ \\ |_) | |  | | (_| |\n");
    printf("_ _`.    \\  |  |  |  /    .'_ _    |___/ .__/|_|  |_|\\__, |\n");
    printf(">_       _} |  |  | {_       _<         |_|           |___/ \n");
    printf(" /. - ~ ,_-'  .^.  `-_, ~ - .\\      \n");
    printf("         '-'|/   \\|`-`              \n\n");
    
    set_log_switch(LOG_ON);
    patch_apply_all();
    hook_install_all();

    ((void (*)(unsigned long))(0x02060B20))(5000); // wait 5 seconds ~

    bldr_handshake();
    
    while(1); // handshake should not return
}