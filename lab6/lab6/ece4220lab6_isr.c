/* ece4220lab6_isr.c
 * ECE4220/7220
 * Author: Luis Alberto Rivera
 
 Basic steps needed to configure GPIO interrupt and attach a handler.
 Check chapter 6 of the BCM2835 ARM Peripherals manual for details about
 the GPIO registers, how to configure, set, clear, etc.
 
 Note: this code is not functional. It shows some of the actual code that you need,
 but some parts are only descriptions, hints or recommendations to help you
 implement your module.
 
 You can compile your module using the same Makefile provided. Just add
 obj-m += YourModuleName.o
 */

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
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/time.h>

MODULE_LICENSE("GPL");

/* Declare your pointers for mapping the necessary GPIO registers.
   You need to map:
   
   - Pin Event detect status register(s)
   - Rising Edge detect register(s) (either synchronous or asynchronous should work)
   - Function selection register(s)
   - Pin Pull-up/pull-down configuration registers
   
   Important: remember that the GPIO base register address is 0x3F200000, not the
   one shown in the BCM2835 ARM Peripherals manual.
*/
#define MSG_SIZE 40
#define CDEV_NAME "lab6"

static unsigned long *BPtr, *GPSEL0, *GPSEL1, *GPSEL2, *GPSET0, *GPCLR0, *GPEDS0, *GPREN0, *GPPUD, 
 *GPPUDCLK;

static struct task_struct *kthread1;
int major;
int mydev_id;	// variable needed to identify the handler
int n = 100;
char buffer [MSG_SIZE];

int my_kthread(void *ptr)//taken from the kthread part one almost exactly
{
	while(1)
	{
		*GPSET0 = *GPSET0 | 0x00000040;
		udelay(n);
		*GPCLR0 = *GPCLR0 | 0x00000040;
		udelay(n);
	
		if(kthread_should_stop())
		{
			printk("thread was exited\n");
			do_exit(0);
		}
	}
	return 0;
}
// Interrupt handler function. Tha name "button_isr" can be different.
// You may use printk statements for debugging purposes. You may want to remove
// them in your final code.

static ssize_t device_read( struct file *filp, char __user *buff, ssize_t lenght, loff_t *offset)
{
	
	ssize_t dummy = copy_to_user(buff, buffer, lenght);
	
	buffer[0] = '\0';

	return 0;

}
static ssize_t device_write( struct file *filp, char __user *buff, size_t len, loff_t *off)
{
	ssize_t  dummy;
	
	if ( len > MSG_SIZE)
	{
	return -EINVAL;
	}

	dummy = copy_from_user(buffer, buff, len);
	
	if( len == MSG_SIZE)
	{
	buffer[len - 1] = '\0';
	}
	else
	{
	buffer[len] = '\0';
	}
	
	switch (buffer[1])
	{
	case 'A':
		printk("button 1");
		n = 300;
		break;

	case 'B':
		printk("button 2");
		n = 600;
		break;
	case 'C':
		printk("button 3");
		n = 900;
		break;
	case 'D':
		printk("button 4");
		n = 1200;
		break;
	case 'E':
		printk("button 5");
		n = 1400;
		break;
	}
	
	return len;


}

static struct  file_operations fops = {
	.write = device_write,
	.read = device_read,
};

static irqreturn_t button_isr(int irq, void *dev_id)
{
	// In general, you want to disable the interrupt while handling it.
	disable_irq_nosync(79);

	// This same handler will be called regardless of what button was pushed,
	// assuming that they were properly configured.
	// How can you determine which button was the one actually pushed?
		
	// DO STUFF (whatever you need to do, based on the button that was pushed)

	// IMPORTANT: Clear the Event Detect status register before leaving.
	switch( *GPEDS0)
	{
		case 0x10000:
			n = 300;
			break;
		case 0x20000:
			n = 600;
			break;
		case 0x40000:
			n = 900;
			break;
		case 0x80000:
			n = 1200;
			break;
		case 0x100000:
			n = 1400;
			break;
	}
	
	*GPEDS0 = *GPEDS0 | 0x001F0000;//clear
//	printk("Interrupt handled\n");	
	enable_irq(79);		// re-enable interrupt
	
    return IRQ_HANDLED;
}

int init_module()
{
	int dummy = 0;
	char kthread[] = "mythread";

	BPtr = (unsigned long *) ioremap( 0x3f200000, 4096);

	// Map GPIO registers
	// Remember to map the base address (beginning of a memory page)
	// Then you can offset to the addresses of the other registers

	// Don't forget to configure all ports connected to the push buttons as inputs.
	
	// You need to configure the pull-downs for all those ports. There is
	// a specific sequence that needs to be followed to configure those pull-downs.
	// The sequence is described on page 101 of the BCM2835-ARM-Peripherals manual.
	// You can use  udelay(100);  for those 150 cycles mentioned in the manual.
	// It's not exactly 150 cycles, but it gives the necessary delay.
	// WiringPi makes it a lot simpler in user space, but it's good to know
	// how to do this "by hand".
		
	// Enable (Async) Rising Edge detection for all 5 GPIO ports.
			
	// Request the interrupt / attach handler (line 79, doesn't match the manual...)
	// The third argument string can be different (you give the name)
	
	GPSEL0 = BPtr;
	GPSET0 = BPtr +7;
	GPCLR0 = BPtr + 10;
	GPSEL1 = BPtr + 1;
	GPSEL2 = BPtr + 2;
	GPEDS0 = BPtr +   16;
	GPREN0 = BPtr + 19;
	GPPUD = BPtr + 37;
	GPPUDCLK = BPtr + 38;
	
	*GPSEL0 = *GPSEL0 | 0x00040000;
	*GPSEL1 = *GPSEL1 & 0xc003ffff;
	*GPSEL2 = *GPSEL2 & 0xfffffff8;
	
	*GPREN0 = *GPREN0 | 0x001f0000;
	
	*GPPUD = *GPPUD | 0x00000001;
	
	*GPPUDCLK = *GPPUDCLK | 0x001f0000;
	udelay(10);
	
	*GPPUD = *GPPUD & 0xfffffffe;
	*GPPUDCLK  = *GPPUDCLK & 0xffe0ffff;
	
	kthread1 = kthread_create( my_kthread, NULL, kthread);
	if(kthread1)
	{
		wake_up_process(kthread1);
	}

	major = register_chrdev( 0, CDEV_NAME, &fops);
	printk("major is: %d", major);
	if(major < 0)
	{
	printk("Registering the character device failed with %d\n", major);
	return major;
	}
	printk("create char device (node) with suko mknod /dev/%s c %d 0\n", CDEV_NAME, major);
	
	dummy = request_irq(79, button_isr, IRQF_SHARED, "Button_handler", &mydev_id);

	printk("Button Detection enabled.\n");
	return 0;
}

// Cleanup - undo whatever init_module did
void cleanup_module()
{
	// Good idea to clear the Event Detect status register here, just in case.
		
	// Disable (Async) Rising Edge detection for all 5 GPIO ports.
	
	// Remove the interrupt handler; you need to provide the same identifier
	kthread_stop( kthread1 );
	*GPEDS0 = *GPEDS0 | 0x001f0000;
	*GPREN0 = *GPREN0 | 0xffe0ffff;
	
	unregister_chrdev(major, CDEV_NAME);

	printk("CHar Device /dev/%s ungregistered\n", CDEV_NAME);
         free_irq(79, &mydev_id);
	
	printk("Button Detection disabled.\n");
}
