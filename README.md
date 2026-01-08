## INFO
This was a project for one of my classes but I wanted to repurpose it for my portfolio

## Project Description
This project is a kernel module that allows you to play tictactoe with a randomized bot algorithm. By compiling and loading the module into the kernel, the user can interact with the device by 
using commands like "echo "PLAY 1,2" > /dev/tictactoe" and can complete a game of tictactoe with the bot. At any point the user can print the results of the board, error messages, and even reset the
game. 

## How to Configure, Compile, and Install the Custom Kernel Module
1. Download code base
2. Run "make"
3. Run "sudo insmod kernelgame.ko"
4. Run "su" and enter password
5. Begin game by running "echo "START (X or O)" > /dev/tictactoe"
6. Place first piece by running "echo "PLAY (X coordinate),(Y coordinate)" > /dev/tictactoe"
7. Have Bot take turn by running "echo "BOT" > /dev/tictactoe"
8. Print board by running "echo "BOARD" > /dev/tictactoe" and then "cat /dev/tictactoe"
9. Take turns back and forth until board is full or someone has won
10. You can see state of errors at any time by typing "cat /dev/tictactoe"

## Known Project Issues
1. None known, one potential issue is that I had inproperly set up my exit function and it somehow
corrupted my git file. I just cloned the repo again and copied over the files and I was able to push to 
github again. I do not expect that this would cause any further issues but I noted it here just in case.

## LLM/AI Prompts Used
1. See the attached error, for some reason my kernel module just started saying Killed, what is going on: Kernel randomly sayed killed for everything including ls, so I asked ChatGPT
what was up so I could debug
2. I am getting an error that my git object is corrupted, what does that mean?: Tried to push to github and it said my object file was corrupted and I wanted to know how to fix so I 
could save my progress to github
3. If I do git clone, will it delete my files if they are newer than the ones on the branch: I was trying to resolve the github issues and was concerned because my local project files were
more updated than my ones on the remote branch, so I was concerned they would get overwritten and lose my progress.

## Sources Used
1. https://tldp.org/LDP/lkmpg/2.6/html/x569.html
2. https://manpages.debian.org/testing/linux-manual-4.8/__class_create.9.en.html
3. https://drive.google.com/drive/folders/12ovK0g7jol_DXe6Sp07_r7mXolHCNrAy
4. https://stackoverflow.com/questions/8697300/how-to-create-a-device-in-dev-automatically-upon-loading-of-the-kernel-module-f
5. https://stackoverflow.com/questions/63448253/file-operation-in-drivers-and-struct-declaration
6. https://lxr.linux.no/linux+v3.10/include/linux/fs.h#L1517
7. https://www.baeldung.com/linux/kernel-module-read-write-files
8. https://www.kernel.org/doc/html/v4.12/core-api/kernel-api.html
9. https://www.linuxquestions.org/questions/programming-9/random-numbers-kernel-642087/#google_vignette
