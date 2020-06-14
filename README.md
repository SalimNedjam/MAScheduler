# MAScheduler
Mutex Aware Scheduler
---------------------------------
## Usage
### Get the v4.19.3 of linux kernel
#### From mirror
$ wget https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tag/?h=v4.19.3


#### Or clone the stable version and downgrade
$ git clone git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
$ cd linux-stable
$ git checkout -b my4.19.3 v4.19.3

### Apply the patch
$ patch -p1 < MAS.patch

### Compile the kernel
$ make -j`nproc`

### We also provide two modules 
- debug: Print debug's messages
- disable: Disable the MAS mecanism
$ insmod debug.ko|disable.ko

### Programs to test and benchmark are also provided
