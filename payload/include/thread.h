/*
 * Copyright (c) 2008-2009 Travis Geiselbrecht
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <list.h>

typedef int status_t;

enum thread_state {
	THREAD_SUSPENDED = 0,
	THREAD_READY,
	THREAD_RUNNING,
	THREAD_BLOCKED,
	THREAD_SLEEPING,
	THREAD_DEATH,
};

typedef int (*thread_start_routine)(void *arg);

/* thread local storage */
enum thread_tls_list {
	MAX_TLS_ENTRY
};

#define THREAD_MAGIC 'thrd'

struct fpstate {
    uint64_t     regs[64];
    uint32_t     fpcr;
    uint32_t     fpsr;
    unsigned int current_cpu;
};

struct arch_thread {
    uintptr_t sp;
    struct fpstate fpstate;
};

typedef struct thread {
	int magic;
	struct list_node thread_list_node;

	/* active bits */
	struct list_node queue_node;
	int priority;
	enum thread_state state;	
	int saved_critical_section_count;
	int remaining_quantum;

	/* if blocked, a pointer to the wait queue */
	struct wait_queue *blocking_wait_queue;
	status_t wait_queue_block_ret;

	/* architecture stuff */
	struct arch_thread arch;

	/* stack stuff */
	void *stack;
	size_t stack_size;

	/* entry point */
	thread_start_routine entry;
	void *arg;

	/* return code */
	int retcode;

	/* thread local storage */
	uint32_t tls[MAX_TLS_ENTRY];

	char name[32];
} thread_t;
