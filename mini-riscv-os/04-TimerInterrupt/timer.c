#include "timer.h"

#define interval 10000000 // cycles; about 1 second in qemu.

#define MAX_TIMER 10
static struct timer timer_list[MAX_TIMER];

static int timer_count = 0;

void timer_init()
{
  // each CPU has a separate source of timer interrupts.
  int id = r_mhartid();

  // ask the CLINT for a timer interrupt.
  *(reg_t*)CLINT_MTIMECMP(id) = *(reg_t*)CLINT_MTIME + interval;

  // set the machine-mode trap handler.
  w_mtvec((reg_t)sys_timer);

  // enable machine-mode interrupts.
  w_mstatus(r_mstatus() | MSTATUS_MIE);

  // enable machine-mode timer interrupts.
  w_mie(r_mie() | MIE_MTIE);
}



// struct timer *timer_create(void (*handler)(void *arg), void *arg, uint32_t timeout)
// {
// 	/* TBD: params should be checked more, but now we just simplify this */
// 	if (NULL == handler || 0 == timeout) {
// 		return NULL;
// 	}

// 	/* use lock to protect the shared timer_list between multiple tasks */
// 	spin_lock();

// 	struct timer *t = &(timer_list[0]);
// 	for (int i = 0; i < MAX_TIMER; i++) {
// 		if (NULL == t->func) {
// 			break;
// 		}
// 		t++;
// 	}
// 	if (NULL != t->func) {
// 		spin_unlock();
// 		return NULL;
// 	}

// 	t->func = handler;
// 	t->arg = arg;
// 	t->timeout_tick = timer_count + timeout;

// 	spin_unlock();

// 	return t;
// }

// void timer_delete(struct timer *timer)
// {
// 	spin_lock();

// 	struct timer *t = &(timer_list[0]);
// 	for (int i = 0; i < MAX_TIMER; i++) {
// 		if (t == timer) {
// 			t->func = NULL;
// 			t->arg = NULL;
// 			break;
// 		}
// 		t++;
// 	}

// 	spin_unlock();
// }

// /* this routine should be called in interrupt context (interrupt is disabled) */
// static inline void timer_check()
// {
// 	struct timer *t = &(timer_list[0]);
// 	for (int i = 0; i < MAX_TIMER; i++) {
// 		if (NULL != t->func) {
// 			if (timer_count >= t->timeout_tick) {
// 				t->func(t->arg);

// 				/* once time, just delete it after timeout */
// 				t->func = NULL;
// 				t->arg = NULL;

// 				break;
// 			}
// 		}
// 		t++;
// 	}
// }

reg_t timer_handler(reg_t epc, reg_t cause)
{
  reg_t return_pc = epc;
  // disable machine-mode timer interrupts.
  w_mie(~((~r_mie()) | (1 << 7)));
  lib_printf("timer_handler: %02d\n", ++timer_count);
  int id = r_mhartid();
  *(reg_t *)CLINT_MTIMECMP(id) = *(reg_t *)CLINT_MTIME + interval;
  // enable machine-mode timer interrupts.
  w_mie(r_mie() | MIE_MTIE);
  return return_pc;
}
