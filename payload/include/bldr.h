#pragma once

#include <stdint.h>
#include <stddef.h>

struct bldr_command_handler {
    void *priv;
    uint32_t attr;
    void *cb;
};

#define BLDR_HANDSHAKE_FUNC  0x02048404
#define BLDR_CALLBACK_FUNC   0x02048818

void bldr_handshake(void);