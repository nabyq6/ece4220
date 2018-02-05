#ifndef MODULE
#define MODULE
#endif

#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/io.h> 

MODULE_LICENSE("GPL");

int init_module(void)
{
	//code to run when it is installed
	int *GPFSEL0 = ioremap(0x3f200000, 32);
	int *GPSET0 = ioremap(0x3f20001C, 32); 
	
	*GPFSEL0 |= (001 << 6);
	*GPSET0 |= ( 1 << 2);
	*GPFSEL0 |= (001 << 9);
	*GPSET0 |= (1 << 3);
	*GPFSEL0 |= (001 << 12);
	*GPSET0 |= ( 1 << 4);
	



	return 0;
}

void cleanup_module(void)
{
	int *GPCLR0 = ioremap( 0x3f200028, 32);
	*GPCLR0 |= ( 1 << 2);
	*GPCLR0 |= ( 1 << 3);
	*GPCLR0 |= ( 1 << 4);		
	printk("MODULE REMOVED");
}
