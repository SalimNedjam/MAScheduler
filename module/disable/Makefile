KERNELDIR_PNL ?= $(PWD)
PWD := $(shell pwd)

obj-m += disable_futex_state.o

all :
	make -C $(KERNELDIR_PNL) M=$(PWD) modules

clean:
	make -C $(KERNELDIR_PNL) M=$(PWD) clean

