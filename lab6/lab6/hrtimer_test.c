/* Based on code found at https://gist.github.com/maggocnx/5946907
   Modified and commented by: Luis Rivera
   
   Compile using the Makefile
 ***** I took alot of cord from kthread_test.c
*/
#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL_
#define __KERNEL_
#endif

//copied over for Kthread
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/interrupt.h>


MODULE_LICENSE("GPL");
unsigned long * GPFSEL0;
unsigned long * GPFSEL1;
unsigned long * GPFSEL2;

unsigned long * BASE;
unsigned long * GPSET;
unsigned long * GPSCLR;

unsigned long * PUD;
unsigned long * PUD_CLK;
unsigned long * EDGE;
unsigned long * EVENT;

int device;
unsigned long timer_interval_ns = 1e6;	// timer interval length (nano sec part)
static struct hrtimer hr_timer;			// timer structure
static int dummy;

static irqreturn_t button_piano( int irq, void *device_id)
{
    // interupt disabled
    disable_irq_nosync(79);
    
    unsigned long result_event = *EVENT & 0x1F0000;
    
    switch(result_event)
    {
        case 0x10000:
            printk("Button 1 pushed");
           timer_interval_ns = 1200000;//takes the place of frequency
            break;
        case 0x20000:
            printk("Button 2 pushed");
           timer_interval_ns = 900000;//takes the place of frequency
            break;
        case 0x40000:
            printk("Button 3 pushed");
          timer_interval_ns  = 600000;//takes the place of frequency
            break;
        case 0x80000:
            printk("Button 4 pushed");
            timer_interval_ns = 400000;//takes the place of frequency
            break;
        case 0x100000:
            printk("Button 5 pushed");
            timer_interval_ns= 200000;//takes the place of frequency
            break;
        default:
            printk("Button choice invalid: case defaulted");
           timer_interval_ns= 0;
            break;
    }
    *EVENT |= 0x1F0000;// clear registor
    enable_irq(79);//enable
    
    return IRQ_HANDLED;
}
// Timer callback function: this executes when the timer expires
enum hrtimer_restart timer_callback(struct hrtimer *timer_for_restart)
{
  	ktime_t currtime, interval;	// time type, in nanoseconds
	unsigned long overruns = 0;
	
	// Re-configure the timer parameters (if needed/desired
  	currtime  = ktime_get();
  	interval = ktime_set(0, timer_interval_ns); // (long sec, long nano_sec)

	// Advance the expiration time to the next interval. This returns how many
	// intervals have passed. More than 1 may happen if the system load is too high.
  	overruns = hrtimer_forward(timer_for_restart, currtime, interval);
	// The following printk only executes once every 1000 cycles.
	if(dummy == 0)
        {
            *GPSET  |= 0x40;
            dummy = 1;
        }
        else if( dummy == 1)
        {
            *GPSCLR |= 0x40;
            dummy = 0;
        }
	return HRTIMER_RESTART;	// Return this value to restart the timer.
							// If you don't want/need a recurring timer, return
							// HRTIMER_NORESTART (and don't forward the timer).
}
int timer_init(void)
{
    int requested;
    //most of this was taken from kthread_trimer
    BASE = ioremap( 0x3F200000, 4096);
    PUD = BASE + 0x94/4;
    PUD_CLK = BASE + 0x98/4;
    
    GPFSEL0 = BASE;
    GPFSEL1 = BASE + 0x4/4;
    GPFSEL2 = BASE + 0x8/4;
    
    GPSET = BASE + 0x1C/4;
    GPSCLR = BASE + 0x28/4;
    
    *GPFSEL0 |= 0x40000; // speaker as output
    *GPFSEL1 &= 0xC003FFFF;//4 buttons
    *GPFSEL2 &= 0xFFFFFFF8;//last button
    
    EDGE = BASE + 0x4C/4;
    EVENT = BASE + 0x40/4;
    
    *PUD |= 0x155;
    udelay(150);
    *PUD_CLK |= 0x1F0000;
    udelay(150);
    *PUD &= 0xFFFFFEAA;//off off
    *PUD_CLK &= 0xFFE0FFFF;//off now
    udelay(150);
    
    //Rising
    *EDGE |= 0x001F0000;
    
    printk("At button detection enabling\n");
    requested = request_irq(79,  button_piano, IRQF_SHARED, "Handles button operation", &device);
    
    
    // Configure and initialize timer
	ktime_t ktime = ktime_set(0, timer_interval_ns); // (long sec, long nano_sec)
	
	// CLOCK_MONOTONIC: always move forward in time, even if system time changes
	// HRTIMER_MODE_REL: time relative to current time.
	hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	
	// Attach callback function to the timer
	hr_timer.function = &timer_callback;
	
	// Start the timer
 	hrtimer_start(&hr_timer, ktime, HRTIMER_MODE_REL);
	return 0;
}
void timer_exit(void)//clearing all event registers and detectors
{
	int ret;
    *EDGE = *EDGE & 0xFFE0FFFF;
    //*EVENT = *EVENT | 0x001f0000;
   
  	ret = hrtimer_cancel(&hr_timer);	// cancels the timer.
  	if(ret)
		printk("The timer was still in use...\n");
	else
		printk("The timer was already canceled...\n");	// if not restarted or
														// canceled before
  	printk("HR Timer module uninstalling\n");
	
}
// Notice this alternative way to define your init_module()
// and cleanup_module(). "timer_init" will execute when you install your
// module. "timer_exit" will execute when you remove your module.
// You can give different names to those functions
module_init(timer_init);
module_exit(timer_exit);
