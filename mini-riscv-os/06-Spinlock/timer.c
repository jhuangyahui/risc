#include "timer.h"
#include "os.h"

// a scratch area per CPU for machine-mode timer interrupts.
reg_t timer_scratch[NCPU][5];

#define interval 10000000 // cycles; about 2 second in qemu.

#define MAX_TIMER 10
static struct timer timer_list[MAX_TIMER];

static int timer_count = 0;

void timer_init()
{
  // each CPU has a separate source of timer interrupts.
  int id = r_mhartid();

  // ask the CLINT for a timer interrupt.
  // int interval = 1000000; // cycles; about 1/10th second in qemu.

  *(reg_t *)CLINT_MTIMECMP(id) = *(reg_t *)CLINT_MTIME + interval;

  // prepare information in scratch[] for timervec.
  // scratch[0..2] : space for timervec to save registers.
  // scratch[3] : address of CLINT MTIMECMP register.
  // scratch[4] : desired interval (in cycles) between timer interrupts.
  reg_t *scratch = &timer_scratch[id][0];
  scratch[3] = CLINT_MTIMECMP(id);
  scratch[4] = interval;
  w_mscratch((reg_t)scratch);

  // enable machine-mode timer interrupts.
  w_mie(r_mie() | MIE_MTIE);
}

struct timer *timer_create(void (*handler)(void *arg), void *arg, uint32_t timeout)
{

  lib_printf("insert into timer_create() !\n");

	/* TBD: params should be checked more, but now we just simplify this */
	if (NULL == handler || 0 == timeout) {
		return NULL;
	}

	/* use lock to protect the shared timer_list between multiple tasks */
	basic_lock();

	struct timer *t = &(timer_list[0]);
	for (int i = 0; i < MAX_TIMER; i++) {
		if (NULL == t->func) {
			break;
		}
		t++;
	}
	if (NULL != t->func) {
		basic_unlock();
		return NULL;
	}

	t->func = handler;
	t->arg = arg;
	t->timeout_tick = timer_count + timeout;

	basic_unlock();

	return t;
}

void timer_delete(struct timer *timer)
{
	basic_lock();

	struct timer *t = &(timer_list[0]);
	for (int i = 0; i < MAX_TIMER; i++) {
		if (t == timer) {
			t->func = NULL;
			t->arg = NULL;
			break;
		}
		t++;
	}

	basic_unlock();
}

/* this routine should be called in interrupt context (interrupt is disabled) */
static inline void timer_check()
{
  lib_printf("insert into timer_check() !\n");
	struct timer *t = &(timer_list[0]);
	for (int i = 0; i < MAX_TIMER; i++) {
		if (NULL != t->func) {
			if (timer_count >= t->timeout_tick) {
				t->func(t->arg);

				/* once time, just delete it after timeout */
				t->func = NULL;
				t->arg = NULL;

				break;
			}
		}
		t++;
	}
}



void timer_handler()
{
  lib_printf("timer_handler: %d\n", ++timer_count);
  timer_check();
  int id = r_mhartid();
  *(reg_t *)CLINT_MTIMECMP(id) = *(reg_t *)CLINT_MTIME + interval;
}
