ifneq ($(KERNELRELEASE),)
	obj-m := motor.o
else
	KERNELDIR := $(EC535)/gumstix/oe/linux-2.6.21/
	PWD := $(shell pwd)
	ARCH := arm
	CROSS := arm-linux-

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS) modules

clean:
	rm -rf *.o *.mod* Module.* .motor* *.ko

endif
