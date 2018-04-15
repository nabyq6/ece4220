
    #ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/sched.h>

int freq = 0;
unsigned long * BASE;
unsigned long * GPFSEL0;
unsigned long * GPFSEL1;
unsigned long * GPFSEL2;
unsigned long * GPSET;
unsigned long * GPSCLR;
unsigned long * PUD;
unsigned long * PUD_CLK;
unsigned long * EDGE;
unsigned long * EVENT;
MODULE_LICENSE("GPL");

int mydev_id;

// structure for the kthread.
static struct task_struct *kthread1;
static irqreturn_t button_isr(int irq, void *dev_id){
	disable_irq_nosync(79); //disable interrupt

	unsigned long result = *EVENT & 0x1F0000;

	switch(result){
	case 0x100000:
		printk("Button 5 pushed\n");
		freq = 200;
		break;
	case 0x80000:
		printk("Button 4 pushed\n");
		freq = 400;
		break;
	case 0x40000:
		printk("Button 3 pushed\n");
		freq = 600;
		break;
	case 0x20000:
		printk("Button 2 pushed\n");
		freq = 800;
		break;
	case 0x10000:
		printk("Button 1 pushed\n");
		freq = 1000;
		break;
	default:
		printk("Case defaulted\n");
		break;
	}

	*EVENT |= 0x1F0000; //clear event register
	enable_irq(79);	//enable interrupt

    return IRQ_HANDLED;
}

// Function to be associated with the kthread; what the kthread executes.
int kthread_fn(void *ptr)
{
	while(1){
		*GPSET |= 0x40;
		udelay(freq);
		*GPSCLR |= 0x40;
		udelay(freq);

		if(kthread_should_stop()) {
			do_exit(0);
		}
	}
	return 0;
}

int thread_init(void)
{
	int requestReturn;
	freq = 1000;
	char kthread_name[15]="kthread_speaker";

	BASE = ioremap(0x3F200000, 4096);
	PUD = BASE + 0x94/4;
	PUD_CLK = BASE + 0x98/4;

	EDGE = BASE + 0x4C/4;
	EVENT = BASE + 0x40/4;

	GPFSEL0 = BASE;
	GPFSEL1 = BASE + 0x4/4;
	GPFSEL2 = BASE + 0x8/4;
	GPSET = BASE + 0x1C/4;
	GPSCLR = BASE + 0x28/4;

	*GPFSEL0 |= 0x40000; //speaker as output
	*GPFSEL1 &= 0xC003FFFF; //first 4 buttons as inputs
	*GPFSEL2 &= 0xFFFFFFF8; //last button as input

	//PUD for buttons
	*PUD |= 0x155;
	udelay(100);
	*PUD_CLK |= 0x1F0000;
	udelay(100);
	*PUD &= 0xFFFFFEAA; //turn off PUD signal
	*PUD_CLK &= 0xFFE0FFFF; //turn off PUD CLK signal
	udelay(100);

	//Rising edge detection
	*EDGE |= 0x1F0000;

	//Request interrupt
	requestReturn = request_irq(79, button_isr, IRQF_SHARED, "Button_handler", &mydev_id);
	printk("Button Detection enabled.\n");
    kthread1 = kthread_create(kthread_fn, NULL, kthread_name);
    if((kthread1))	// true if kthread creation is successful
    {
        wake_up_process(kthread1);
    }

    return 0;
}

void thread_cleanup(void) {
	int ret;
	free_irq(79, &mydev_id);
	ret = kthread_stop(kthread1);

	if(!ret)
		printk("Kthread stopped\n");
}

// Notice this alternative way to define your init_module()
// and cleanup_module(). "thread_init" will execute when you install your
// module. "thread_cleanup" will execute when you remove your module.
// You can give different names to those functions.
module_init(thread_init);
module_exit(thread_cleanup);
