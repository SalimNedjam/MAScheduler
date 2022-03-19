PWD := $(shell pwd)
KERNELDIR_PNL ?= $(PWD)

obj-m += debug_futex_state.o

all :
	make -C $(KERNELDIR_PNL) M=$(PWD) modules

clean:
	make -C $(KERNELDIR_PNL) M=$(PWD) clean

