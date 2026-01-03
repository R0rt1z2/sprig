#include <debug.h>
#include <bldr.h>

typedef int (*handshake_fn_t)(struct bldr_command_handler *);

void bldr_handshake(void)
{
    struct bldr_command_handler handler = {
        .priv = NULL,
        .attr = 0,
        .cb = (void *)BLDR_CALLBACK_FUNC
    };
    
    handshake_fn_t handshake_fn = (handshake_fn_t)BLDR_HANDSHAKE_FUNC;
    
    printf("About to handshake...\n\n");
    handshake_fn(&handler);
}