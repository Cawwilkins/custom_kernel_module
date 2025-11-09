#include <linux/module.h> // Needed for all kernel modules
#include <linux/kernel.h> // Needed for KERN_INFO
#include <linux/init.h>   // Needed for macros like __init and __exit
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/string.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/moduleparam.h>
#include <linux/random.h>

static int major;
static int gameStarted = 0;  // treated as bool
static int playerTurn = 1;   // treated as bool
static int gameOver = 0;     // treated as bool
static char playerPiece;
static char botPiece;
static struct class *cl;
static char board[4][4];
static char error_msgs[100];


void board_cmd(char* buffer) {
        int i = 0;
        int j = 0;
	int pos = 0;

        // If params added, board incorrect
        if (strcmp(buffer, "BOARD") != 0){
                strcpy(error_msgs, "INVALID_BOARD_CMD");
                return;

        // No other params but game not started
        } else if (gameStarted == 0) {
                strcpy(error_msgs, "GAME_NOT_STARTED");
                return;

        // Invalid call
        } else {
                strcpy(error_msgs, " ");
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (j == 0 && i == 0) {
					pos += snprintf(error_msgs + pos, sizeof(error_msgs) - pos, "%c ", board[i][j]);
				} else if (j == 0 || i == 0) {
					pos += snprintf(error_msgs + pos, sizeof(error_msgs) - pos, "%d ", board[i][j]);
				} else {
            				pos += snprintf(error_msgs + pos, sizeof(error_msgs) - pos, "%c ", board[i][j]);
        			}
			}
        		pos += snprintf(error_msgs + pos, sizeof(error_msgs) - pos, "\n");
		}
        }
	return;
}


int check_gamestate_bot(void) {
        int i;
        int j;
        int counter = 0;

	// Check if board full
	for (i = 1; i <= 3; i++) {
                for (j = 1; j <= 3; j++) {
                        if (board[i][j] == '_') {
                                counter++;
                        }
                }
	}
        if (counter == 0){
		gameOver = 1;
      	        return 1;
        } else {
                counter = 0;
        }

        // Check each row
        for (i = 1; i <= 3; i++) {
                for (j = 1; j <= 3; j++) {
                        if (board[i][j] == botPiece) {
                                counter++;
                        }
                }
                if (counter == 3){
			gameOver = 1;
                        return 1;
                } else {
                        counter = 0;
                }
        }

        // Check each column
        for (i = 1; i <= 3; i++) {
                for (j = 1; j <= 3; j++) {
                        if (board[j][i] == botPiece) {
                                counter++;
                        }
                }
                if (counter == 3){
			gameOver = 1;
                        return 1;
                } else {
                        counter = 0;
                }
        }

        // Check each diagonal
        for (i = 1; i <= 3; i++) {
                if (board[i][i] == botPiece) {
                        counter++;
                }
        }
        if (counter == 3){
		gameOver = 1;
                return 1;
        } else {
                counter = 0;
        }
        for (i = 1; i <= 3; i++) {
                if (board[i][4 - i] == botPiece) {
                        counter++;
        	}
	}
        if (counter == 3){
		gameOver = 1;
                return 1;
        } else {
                counter = 0;
        }
        return 0;
}

int check_gamestate_player(void) {
	int i;
	int j;
	int counter = 0;

	// Check if board full
        for (i = 1; i <= 3; i++) {
                for (j = 1; j <= 3; j++) {
                        if (board[i][j] == '_') {
                                counter++;
                        }
                }
        }
	if (counter == 0){
        	gameOver = 1;
		return 1;
        } else {
               	counter = 0;
        }

	// Check each row
	for (i = 1; i <= 3; i++) {
		for (j = 1; j <= 3; j++) {
			if (board[i][j] == playerPiece) {
				counter++;
			}
		}
		if (counter == 3){
			gameOver = 1;
			return 1;
		} else {
			counter = 0;
		}
	}

	// Check each column
	for (i = 1; i <= 3; i++) {
                for (j = 1; j <= 3; j++) {
                        if (board[j][i] == playerPiece) {
                                counter++;
                        }
                }
                if (counter == 3){
			gameOver = 1;
                        return 1;
                } else {
                        counter = 0;
                }
        }

	// Check each diagonal
	for (i = 1; i <= 3; i++) {
        	if (board[i][i] == playerPiece) {
	                counter++;
                }
	}
        if (counter == 3){
		gameOver = 1;
        	return 1;
      	} else {
        	counter = 0;
      	}
	for (i = 1; i <= 3; i++) {
                if (board[i][4 - i] == playerPiece) {
                        counter++;
                }
        }
        if (counter == 3){
		gameOver = 1;
                return 1;
        } else {
                counter = 0;
        }
	return 0;
}

void bot_cmd(char* buffer) {
	int x;
	int y;

        // Check if game has started
        if (gameStarted == 0) {
                strcpy(error_msgs, "GAME_NOT_STARTED");
                return;
        // Check if no other params given
        } else if (strcmp(buffer, "BOT") != 0){
                strcpy(error_msgs, "INVALID_BOT");
                return;
        // Check if game over
        } else if (gameOver == 1) {
                strcpy(error_msgs, "GAME_OVER");
                return;
        // Check if it is CPU's turn
        } else if (playerTurn == 1) {
                strcpy(error_msgs, "NOT_CPU_TURN");
                return;
	}

        // Keep looping until there is a free spot
        do {
		// Generate random numbers between 1-3
		get_random_bytes(&x, sizeof(x));
		x = (x % 3) + 1;
		get_random_bytes(&y, sizeof(y));
		y = (y % 3) + 1;
        } while (board[x][y] != '_');

	// Set board
	board[x][y] = botPiece;

	// Check if gameover and pass correct message
        if (check_gamestate_bot() == 1) {
        	strcpy(error_msgs, "GAME_OVER");
       	} else {
		strcpy(error_msgs, "OK");
	}

	// Now player's turn
        playerTurn = 1;
	return;
}

void play_cmd(char* buffer) {
	char* temp = buffer;
        char *words;
	char *x_string;
	long x_coord;
	long y_coord;

	// Check if game has started
	if (gameStarted == 0) {
		strcpy(error_msgs, "GAME_NOT_STARTED");
		return;
        // Check if no other params given
        } else if (strcmp(buffer, "PLAY") == 0){
                strcpy(error_msgs, "INVALID_FORMAT");
                return;
	// Check if game over
	} else if (gameOver == 1) {
		strcpy(error_msgs, "GAME_OVER");
		return;
	// Make sure the format is correct
	} else if (!strstr(buffer, "PLAY ") || !(strstr(buffer, ","))) {
		strcpy(error_msgs, "OUT_OF_BOUNDS");
                return;
	}

	// Now we are guaranteed to have coords, so parse time
	words = strsep(&temp, " ");
	x_string = strsep(&temp, ",");

	if(kstrtol(x_string, 10, &x_coord) != 0) {
		strcpy(error_msgs, "OUT_OF_BOUNDS");
		return;
	};

	// Temp is now ystring
	if (kstrtol(temp, 10, &y_coord) != 0) {
		strcpy(error_msgs, "OUT_OF_BOUNDS");
		return;
	};

	// Check if out of bounds
	if (x_coord > 3 || y_coord > 3 || x_coord < 1 || y_coord < 1) {
		strcpy(error_msgs, "OUT_OF_BOUNDS");
		return;
	// Check if something already there
	} else if (board[x_coord][y_coord] != '_') {
		strcpy(error_msgs, "CANNOT_PLACE");
		return;
	// Check if not players turn
        } else if (playerTurn == 0) {
                strcpy(error_msgs, "NOT_PLAYER_TURN");
                return;
	// Place
	} else {
		board[x_coord][y_coord] = playerPiece;
		if (check_gamestate_player() == 1) {
			strcpy(error_msgs, "GAME_OVER");
		} else {
			strcpy(error_msgs, "OK");
		}
		// Now CPU turn
		playerTurn = 0;
		return;
	}
	return;
}


void reset_cmd(char* buffer) {
        int i = 0;
        int j = 0;

        // No other params and game started, pass
        if (strcmp(buffer, "RESET") == 0 && gameStarted == 1){
		for (i = 0; i < 4; i++){
                        for (j = 0; j < 4; j++){
				if (i == 0 && j == 0) {
					board[i][j] = '.';
				} else if (i == 0) {
					board[i][j] = j;
				} else if (j == 0) {
					board[i][j] = i;
				} else {
					board[i][j] = '_';
				}
                        }
                }
                gameStarted = 0;
                strcpy(error_msgs, "OK");
                return;

        // No other params but game not started
	} else if (strcmp(buffer, "RESET") == 0 && gameStarted == 0) {
		strcpy(error_msgs, "GAME_NOT_STARTED");
		return;

	// Invalid call
	} else {
		strcpy(error_msgs, "INVALID_RESET");
		return;
	}
	return;
}


void start_cmd(char* buffer) {
	char* temp = buffer;
        char *word;
        char *words[2];
        int i = 0;
	int j = 0;

	// Check if no other params given
	if (strcmp(buffer, "START") == 0){
                strcpy(error_msgs, "MISSING_PIECE");
                return;
        }

	// Separate the command
        while ((word = strsep(&temp, " ")) != NULL && i < 2) {
                words[i++] = word;
        }
	// Check if valid piece
        if (strcmp(words[1], "X") != 0 && strcmp(words[1], "O") != 0) {
                strcpy(error_msgs, "INVALID_PIECE");
                return;
        }
	// Check if game already started
        if (gameStarted == 1){
                strcpy(error_msgs, "GAME_STARTED");
                return;
        }
	// Start the game
        else {
                // Build gameboard
		for (i = 0; i < 4; i++){
                        for (j = 0; j < 4; j++){
                                if (i == 0 && j == 0) {
                                        board[i][j] = '.';
                                } else if (i == 0) {
                                        board[i][j] = j;
                                } else if (j == 0) {
                                        board[i][j] = i;
                                } else {
                                        board[i][j] = '_';
                                }
                        }
                }
		gameStarted = 1;
                playerTurn = 1;
                gameOver = 0;
                playerPiece = words[1][0];
                if (playerPiece == 'X') {
                        botPiece = 'O';
                } else {
                        botPiece = 'X';
                }
                strcpy(error_msgs, "OK");
                return;

	}
	return;
}


ssize_t read_file(struct file *filep, char __user *buffer, size_t len, loff_t *offset) {
	char formatted[100];

	// If already read, return
	if (*offset >= strlen(error_msgs)){
		return 0;
	}

	// To append a newline to the messages
	snprintf(formatted, sizeof(formatted), "%s\n", error_msgs);

	// Copy formatted message to user_space to print
	if(copy_to_user(buffer, formatted, strlen(formatted))){
		printk("Error copying to user space");
		return -1;
	}

	// Keep track of where we have read
	*offset += strlen(formatted);
	return strlen(formatted);
}


ssize_t write_file(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
	// Take in user input, and write to /dev/ corresponding action
	char kern_buffer[100];

	// Get user input into kernel space
	if (copy_from_user(kern_buffer, buffer, len)){
		strcpy(error_msgs, "KERN_COPY_ERR");
		return 0;
	}

	// Null terminate
	kern_buffer[len] = '\0';
	if(kern_buffer[len - 1] == '\n') {
		kern_buffer[len - 1] = '\0';
	}

	// Controller
	if (strstr(kern_buffer, "START")){
		start_cmd(kern_buffer);
		return len;
	} else if (strstr(kern_buffer, "RESET")){
		reset_cmd(kern_buffer);
		return len;
	} else if (strstr(kern_buffer, "PLAY")){
		play_cmd(kern_buffer);
		return len;
	} else if (strstr(kern_buffer, "BOT")){
		bot_cmd(kern_buffer);
		return len;
	} else if (strstr(kern_buffer, "BOARD")){
		board_cmd(kern_buffer);
		return len;
	} else {
		strcpy(error_msgs, "CMD_NOT_RECOGNIZED");
		return len;
	}
}


/**
 * Structure to represent what happens when you read and write to your driver.
 *
 *  Note: you have to pass this in when registering your character driver.
 *        initially, this is not done for you.
 */
static struct file_operations char_driver_ops = {
  .read   = read_file,
  .write  = write_file
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
  	struct device *dev_real;
	dev_t dev;

	// Register and store major number
	major = register_chrdev(0, "tictactoe", &char_driver_ops);

	cl = class_create(THIS_MODULE, "tictactoe");

	dev = MKDEV(major, 0);
	dev_real = device_create(cl, NULL, dev, NULL, "tictactoe");

	printk(KERN_INFO "TicTacToe loaded.\n");
	return 0;
}

/**
 * Cleans up memory and unregisters your module.
 *  - cleanup: freeing memory.
 *  - unregister: remove your entry from /dev.
 */
static void __exit kernel_game_exit(void) {

  	// -- cleanup memory --

	unregister_filesystem(&kernel_game_driver);

	device_destroy(cl, MKDEV(major, 0));
	class_destroy(cl);
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
