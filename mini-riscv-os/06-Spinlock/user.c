#include "os.h"
#include "timer.h"

int shared_var = 500;

#define DELAY 4000

lock_t lock;

struct userdata {
	int counter;
	char *str;
};

/* Jack must be global */
struct userdata person = {0, "Jack"};

void timer_func(void *arg)
{
	if (NULL == arg)
		return;

	struct userdata *param = (struct userdata *)arg;

	param->counter++;
	lib_printf("======> TIMEOUT: %s: %d\n", param->str, param->counter);
}

void user_task0(void)
{
	lib_puts("Task0: Created!\n");

	struct timer *t1 = timer_create(timer_func, &person, 3);
	if (NULL == t1) {
		lib_printf("timer_create() failed!\n");
	}
	struct timer *t2 = timer_create(timer_func, &person, 5);
	if (NULL == t2) {
		lib_printf("timer_create() failed!\n");
	}
	struct timer *t3 = timer_create(timer_func, &person, 7);
	if (NULL == t3) {
		lib_printf("timer_create() failed!\n");
	}
	while (1)
	{
		lib_puts("Task0: Running...\n");
		lib_delay(DELAY);
	}
}

void user_task1(void)
{
	lib_puts("Task1: Created!\n");
	while (1)
	{
		lib_puts("Task1: Running...\n");
		lib_delay(DELAY);
	}
}

void user_task2(void)
{
	lib_puts("Task2: Created!\n");
	while (1)
	{
		for (int i = 0; i < 50; i++)
		{
			lock_acquire(&lock);
			shared_var++;
			lock_free(&lock);
			lib_delay(DELAY);
		}
		lib_printf("The value of shared_var is: %d \n", shared_var);
	}
}

void user_task3(void)
{
	lib_puts("Task3: Created!\n");
	while (1)
	{
		lib_puts("Tryin to get the lock... \n");
		lock_acquire(&lock);
		lib_puts("Get the lock!\n");
		lock_free(&lock);
		lib_delay(DELAY);
	}
}

void user_init()
{
	lock_init(&lock);
	task_create(&user_task0);
	task_create(&user_task1);
	// task_create(&user_task2);
	// task_create(&user_task3);
}
