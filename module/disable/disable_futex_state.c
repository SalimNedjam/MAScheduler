#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/futex.h>

MODULE_DESCRIPTION("Module to disable futex state mecanism");
MODULE_AUTHOR("Pierre-Loup Gosse");
MODULE_LICENSE("GPL");

static int __init disable_init(void)
{
  FUTEX_STATE_ENABLE = 0;
  pr_info("Futex state is disable :(\n");

	return 0;
}
module_init(disable_init);

static void __exit disable_exit(void)
{
  FUTEX_STATE_ENABLE = 1;
  pr_info("Futex state is enable :D\n");
}
module_exit(disable_exit);
