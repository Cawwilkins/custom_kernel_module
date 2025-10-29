#include <linux/module.h> // Needed for all kernel modules
#include <linux/kernel.h> // Needed for KERN_INFO
#include <linux/init.h>   // Needed for macros like __init and __exit
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/moduleparam.h>
#include <linux/random.h>


/**
 * Structure to represent what happens when you read and write to your driver.
 *
 *  Note: you have to pass this in when registering your character driver.
 *        initially, this is not done for you.
 */
static struct file_operations char_driver_ops = {
  .read   = NULL,
  .write  = NULL
};

// You only need to modify the name here.
static struct file_system_type kernel_game_driver = {
  .name     = "bogus_file_system_name_you_will_lose_points",
  .fs_flags = O_RDWR
};

/**
 * Initializes and Registers your Module. 
 * You should be registering a character device,
 * and initializing any memory for your project.
 * Note: this is all kernel-space!
 * 
 */
static int __init kernel_game_init(void) {

  // -- register your character device here --

  return register_filesystem(&kernel_game_driver);
}

/**
 * Cleans up memory and unregisters your module.
 *  - cleanup: freeing memory.
 *  - unregister: remove your entry from /dev.
 */
static void __exit kernel_game_exit(void) {

  // -- cleanup memory --
  
  // -- unregister your device driver here --
  
  return;
}

module_init(kernel_game_init);
module_exit(kernel_game_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("yournamehere");
MODULE_DESCRIPTION("writeonesentence");
