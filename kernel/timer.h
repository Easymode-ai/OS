//timer.h
#include "library/e9print.h"
#include "structs.h"
#include "display.h"

void timer_phase(int hz)
{
    int divisor = 1193180 / hz;         /* Calculate our divisor */
    write_port(0x43, 0x36);             /* Set our command byte 0x36 */
    write_port(0x40, divisor & 0xFF);   /* Set low byte of divisor */
    write_port(0x40, divisor >> 8);     /* Set high byte of divisor */
}

int timer_ticks = 0;
void timer_handler(registers_t r)
{
	//timer_phase(100);
    timer_ticks++;

    /* Every 18 clocks (approximately 1 second), we will
    *  display a message on the screen */
	
	if (timer_ticks % 2 == 0)
    {
	//UpdateScreen();
	}
	
    if (timer_ticks % 18 == 0)
    {

	//	int rnd = Random_next();
	//	e9_printf("rnd : %d",rnd);
	
    }
	//else
	//e9_printf("LOW");
}
uint64_t start =0;


uint64_t GetTicks()
{
	
uint32_t low, high;
    asm volatile("rdtsc":"=a"(low),"=d"(high));
    return ((uint64_t)high << 32) | low;
}
void TimerStart(){
	start= timer_ticks;
}

uint64_t GetTick()
{
	return (timer_ticks - start);
}
void printticks()
{
	e9_printf("%d", timer_ticks);
}