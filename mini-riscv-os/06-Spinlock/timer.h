#ifndef __TIMER_H__
#define __TIMER_H__

#include "riscv.h"
#include "sys.h"
#include "lib.h"
#include "task.h"

extern void timer_handler();
extern void timer_init();

struct timer {
	void (*func)(void *arg);
	void *arg;
	uint32_t timeout_tick;
};
extern struct timer *timer_create(void (*handler)(void *arg), void *arg, uint32_t timeout);
extern void timer_delete(struct timer *timer);


#endif
