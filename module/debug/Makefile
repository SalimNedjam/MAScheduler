KERNELDIR_PNL ?= /home/pierre-loup/fac/PSAR/MAScheduler/linux-4.19.3
PWD := $(shell pwd)

obj-m += debug_futex_state.o

all :
	make -C $(KERNELDIR_PNL) M=$(PWD) modules

clean:
	make -C $(KERNELDIR_PNL) M=$(PWD) clean

