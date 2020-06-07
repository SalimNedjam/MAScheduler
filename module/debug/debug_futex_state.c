#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/futex.h>

MODULE_DESCRIPTION("Module to enable debug futex state mecanism");
MODULE_AUTHOR("Pierre-Loup Gosse");
MODULE_LICENSE("GPL");

static int __init debug_init(void)
{
  FUTEX_STATE_DEBUG = 1;
  pr_info("Futex state debug is enable :o\n");

	return 0;
}
module_init(debug_init);

static void __exit debug_exit(void)
{
  FUTEX_STATE_DEBUG = 0;
  pr_info("Futex state debug is disable :$\n");
}
module_exit(debug_exit);
