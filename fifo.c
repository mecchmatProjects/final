
// C program to implement 2 of FIFO 
// This side reads first, then reads 
#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
  
int main() 
{ 
    int fd1; 
  
    // FIFO file path 
    char myfifo1[20] = "/home/box/in.fifo"; 

    // FIFO file path 
    char myfifo2[20] = "/home/box/out.fifo"; 
    

    // Creating the named file(FIFO) 
    // mkfifo(<pathname>,<permission>) 
    mkfifo(myfifo1, 0666); 
  
    mkfifo(myfifo1, 0666); 

    char str1[255]; 
    while (1) 
    { 
        // First open in read only and read 
        fd1 = open(myfifo1,O_RDONLY); 
        read(fd1, str1, 255); 
  
        // Print the read string and close 
        fprintf(stderr, "User1: %s\n", str1); 
        close(fd1); 
  
        // Now open in write mode and write 
        fd2 = open(myfifo2,O_WRONLY); 
        //fgets(str2, 80, stdin); 
        write(fd2, str1, strlen(str1)+1); 
        close(fd2); 
    } 
    return 0; 
} 

