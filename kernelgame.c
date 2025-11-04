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

static int major;
static struct class c_dev;
static struct dev_t dev;

void read_file() {
	// Where we will read the file
}

void write_file() {
	// where we will write the file
}

/**
 * Structure to represent what happens when you read and write to your driver.
 *
 *  Note: you have to pass this in when registering your character driver.
 *        initially, this is not done for you.
 */
static struct file_operations char_driver_ops = {
  .read   = read_file();
  .write  = write_file();
};

// You only need to modify the name here.
static struct file_system_type kernel_game_driver = {
  .name     = "tictactoe",
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
	printk(KERN_INFO "TicTacToe loaded.\n");
	// Register and store major number
	major = register_chrdev(0, "tictactoe", &char_driver_ops);
	cl = class_create(kernel_game_driver, "tictactoe");
	device_create(cl, NULL, dev, NULL, "d");

	return register_filesystem(&kernel_game_driver);
}

/**
 * Cleans up memory and unregisters your module.
 *  - cleanup: freeing memory.
 *  - unregister: remove your entry from /dev.
 */
static void __exit kernel_game_exit(void) {

  	// -- cleanup memory --
	unregister_chrdev(major, "tictactoe");
  	// -- unregister your device driver here --
	printk(KERN_INFO "Exiting");
  	return;
}

module_init(kernel_game_init);
module_exit(kernel_game_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Christian Wilkins");
MODULE_DESCRIPTION("This module implements tic-tac-toe in the kernel.");
