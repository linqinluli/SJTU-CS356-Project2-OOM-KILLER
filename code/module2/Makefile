obj-m := set_mm_limit.o
KID := /home/linqinluli/os/android-kernel/kernel/goldfish
CROSS_COMPILE=arm-linux-androideabi-
CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld

KBUILD_EXTRA_SYMBOLS += /home/linqinluli/osprj2/module1/Module.symvers

export KBUILD_EXTRA_SYMBOLS
all:
	make -C $(KID) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) M=$(shell pwd) modules

clean:
	rm -rf *.ko *.o *.mod.c *.order *.symvers
