
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
    int fd1,fd2; 
  
    // FIFO file path 
    char myfifo1[20] = "/home/box/in.fifo"; 

    // FIFO file path 
    char myfifo2[20] = "/home/box/out.fifo"; 
    

    // Creating the named file(FIFO) 
    // mkfifo(<pathname>,<permission>) 
    mkfifo(myfifo1, 0666); 
  
    mkfifo(myfifo2, 0666); 

   fd1 = open(myfifo1,O_RDONLY); 
   fd2 = open(myfifo2,O_WRONLY); 
    char str1[255]; 
   
  int len=0;
  while(1){
    len = read(pipe_in, &buf, sizeof(buf));
    write(pipe_out, &buf, len);
   }

   close(fd1);
   close(fd2);
    return 0; 
} 

