#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Robert W. Oliver II");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");

static int __init
/**
 * Load时候触发的函数
 */
example_init(void) {
    printk(KERN_INFO
    "Hello, World!\n");
    return 0;
}

static void __exit
/**
 * Unload时候触发的函数
 */
example_exit(void) {
    printk(KERN_INFO
    "Goodbye, World!\n");
}

module_init(example_init);
module_exit(example_exit);