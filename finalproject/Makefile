#obj-m += kthread_test.o
#obj-m += hrtimer_test.o
obj-m += unit_kernel.o
#obj-m += Lab6_cdev_user.c

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
