/* Based on example from: http://tuxthink.blogspot.com/2011/02/kernel-thread-creation-1.html
   Modified and commented by: Luis Rivera			
   
   Compile using the Makefile
*/

#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif
   
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>	// for kthreads
#include <linux/sched.h>	// for task_struct
#include <linux/time.h>		// for using jiffies 
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
int frequency = 0; // intialized to to default case 

// structure for the kthread.
static struct task_struct *kthread1;
static irqreturn_t button_piano( int irq, void *device_id)
{	
	// interupt disabled 
	disable_irq_nosync(79);
	
	unsigned long result_event = *EVENT & 0x1F0000;

	switch(result_event) 
	{
		case 0x10000:
			printk("Button 1 pushed");
			frequency = 1200;
			break; 
		case 0x20000:
			printk("Button 2 pushed");
			frequency  = 900;
			break;
		case 0x40000:
			printk("Button 3 pushed");
			frequency  = 600;
			break; 
		case 0x80000:
			printk("Button 4 pushed");
			frequency  = 400;
			break;
		case 0x100000:
			printk("Button 5 pushed");
			frequency = 200;
			break; 
		default:
			printk("Button choice invalid: case defaulted");
			frequency = 0;
			break;	
	}
	*EVENT |= 0x1F0000;
	enable_irq(79);

	return IRQ_HANDLED;
}
// Function to be associated with the kthread; what the kthread executes.
int kthread_fn(void *ptr)
{
	printk("In kthread1 new mod\n");
	while(1)
	{
		// good for > 10 ms
		//msleep_interruptible(1000); // good for > 10 ms
		//udelay(unsigned long usecs);	// good for a few us (micro s)
		//usleep_range(unsigned long min, unsigned long max); // good for 10us - 20 ms
		*GPSET  |= 0x40;
		udelay(frequency);
		
		// In an infinite loop, you should check if the kthread_stop
		// function has been called (e.g. in clean up module). If so,
		// the kthread should exit. If this is not done, the thread
		// will persist even after removing the module.
		*GPSCLR |= 0x40;
		udelay(frequency);

		if(kthread_should_stop()) {
			do_exit(0);
		}
				
		// comment out if your loop is going "fast". You don't want to
		// printk too often. Sporadically or every second or so, it's okay.
	}
	
	return 0;
}

int thread_init(void)
{
	char kthread_name[40]="my_kthread for the speaker";
	// try running  ps -ef | grep my_kthread
										// when the thread is active.
	printk("In init module\n");
    	    
    	int requested;
	frequency = 1200;

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
	
	requested = request_irq(79,  button_piano, IRQF_SHARED, "Handles button operation", &device);
	printk("Buttons enabled");

	kthread1 = kthread_create(kthread_fn, NULL, kthread_name);
	
    if((kthread1))	// true if kthread creation is successful
    {
        printk("Inside if\n");
		// kthread is dormant after creation. Needs to be woken up
        wake_up_process(kthread1);
    }

    return 0;
}

void thread_cleanup(void) {
	int ret;
	ret = kthread_stop(kthread1);
	free_irq(79, &device);								
	if(!ret)
		printk("Kthread stopped\n");
}

// Notice this alternative way to define your init_module()
// and cleanup_module(). "thread_init" will execute when you install your
// module. "thread_cleanup" will execute when you remove your module.
// You can give different names to those functions.
module_init(thread_init);
module_exit(thread_cleanup);
