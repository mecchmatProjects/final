#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>


#define READ   0
#define WRITE  1

#define DO_OR_DIE(x, s) do { \
    if ((x) < 0) { \
        perror(s); \
        exit(1);   \
    } \
} while (0)

#define OUTFILE "outfile1.txt" //"home/box/result.out"

#define MAX_COMM 20

#define DEBUG 1

int exec_seria(char* commands[], char* params[], int size){

  fprintf(stderr,"connect size %d for command %s %s", size, commands[0], params[0]);
  int fd[MAX_COMM][2];  
   //pipe (fd); /* Create an unnamed pipe */ 
  int pid[MAX_COMM];

  if(size==0) {
    DO_OR_DIE(pipe(fd[0]), "Pipe 0 error");
    close (fd[0][READ]); /* Close unused end */



     dup2 (fd[0][WRITE],STDOUT_FILENO); /* Duplicate used end to stdout */
    int file1 = open(OUTFILE, O_WRONLY | O_CREAT|O_TRUNC, S_IRUSR | S_IWUSR); 
     dup2 (file1, STDOUT_FILENO); /* Duplicate used end to file */ 
     close (fd[0][WRITE]); /* Close original used end */
     close(file1);
     //close(STDOUT_FILENO);
     //close(STDIN_FILENO);
#if DEBUG
     fprintf(stderr, "connect1 %d %d %s %s",fd[0][0], fd[0][1], commands[0], params[0] ); 
#endif

   /* Execute program */ 
     DO_OR_DIE(execlp (commands[0], commands[0], params[0], NULL),
                 " The only command not works "); 
     return 0;
  }

    DO_OR_DIE(pipe(fd[0]), "Pipe 0 error");

    DO_OR_DIE(pid[0]=fork(),"Fork 0 error");
    
    if(pid[0]==0) { // child 0
      // input from stdin (already done)
      close(STDIN_FILENO);
      // output to pipe1    
     close (fd[0][READ]); /* Close unused end */

     dup2 (fd[0][WRITE],STDOUT_FILENO); /* Duplicate used end to stdout */ 

#if DEBUG
     char buf[20];
     if(size==1) strcpy(buf,OUTFILE);
     else strcpy(buf, "outfile000.txt");
     int file1 = open(buf, O_WRONLY | O_CREAT|O_TRUNC, S_IRUSR | S_IWUSR);
     dup2 (file1,STDOUT_FILENO); /* Duplicate used end to stdout */ 
     close(file1);
     fprintf(stderr, "connect0 %d %d %s\n",fd[0][0], fd[0][1], commands[0]); 
#endif
     close (fd[0][WRITE]); /* Close original used end */
        /* Execute writer program */ 

     //close(STDOUT_FILENO);
     

     DO_OR_DIE(execlp (commands[0], commands[0], params[0], NULL),
                 " The first command not works ");
  // exec didn't work, exit

    }
	
   // parent

   //close(STDIN_FILENO);
   //close(STDOUT_FILENO);

   char error_text[20];
   char tmp_file_name_in[20];
   char tmp_file_name_out[20];

   for(int i=1;i<size-1;++i){

     waitpid( pid[i-1], NULL, 0);    
     snprintf(error_text, 20, "Pipe %d error",i);
     DO_OR_DIE(pipe(fd[i]), error_text);

     snprintf(error_text, 20, "Fork %d error",i);
     DO_OR_DIE(pid[i]=fork(), error_text);
    
    if(pid[i]==0) { // child 

     dup2 (fd[i][READ],STDIN_FILENO); /* Duplicate read end to stdin */ 
     dup2 (fd[i][WRITE], STDOUT_FILENO);/* Duplicate write end to stdout */ 
    

#if DEBUG

     snprintf(tmp_file_name_in, 20, "outfile00%d.txt",i-1);
     snprintf(tmp_file_name_out, 20, "outfile00%d.txt",i);
     int file1 = open(tmp_file_name_in, O_RDONLY, S_IRUSR | S_IWUSR);
     int file2 = open(tmp_file_name_out, O_WRONLY| O_CREAT|O_TRUNC, S_IRUSR | S_IWUSR);

     dup2 (file1,STDIN_FILENO); /* Duplicate used end to stdin */ 
     dup2 (file2, STDOUT_FILENO);

    
     //fprintf(stderr, "connect %d %d %d :%s\n", i, fd[i][0], fd[i][1], commands[i]); 
     close(file1);
     close(file2);
#endif
     close (fd[i-1][READ]); /* Close original used end */
     close (fd[i-1][WRITE]); /* Close original used end */
     close (fd[i][READ]); /* Close original used end */
     close (fd[i][WRITE]); /* Close original used end */
  
     

        /* Execute writer program */ 
     DO_OR_DIE(execlp (commands[i], commands[i], params[i], NULL),
                 " The command not works ");
  // exec didn't work, exit

    } // end child

  }
   
     waitpid(pid[size-2], NULL, 0);

     DO_OR_DIE(pipe(fd[size-1]), "Last pipe error");

     DO_OR_DIE(pid[size-1]=fork(), "Last fork error");

    if(pid[size-1]==0) { // child 

    
#if DEBUG
     snprintf(tmp_file_name_in, 20, "outfile00%d.txt",size-2);
     int file1 = open(tmp_file_name_in, O_RDONLY, S_IRUSR | S_IWUSR);
    
#endif

    // fprintf(stderr, "connect last %d %d %d %s\n", size-1, fd[size-1][0], fd[size-1][1], commands[size-1]); 
     int file2 = open(OUTFILE, O_WRONLY| O_CREAT|O_TRUNC, S_IRUSR | S_IWUSR);

     dup2 (fd[size-1][READ], STDIN_FILENO); /* Duplicate read end to stdin */ 
     dup2 (fd[size-1][WRITE], STDOUT_FILENO);/* Duplicate write end to stdout */ 

#if  DEBUG
     dup2 (file1, STDIN_FILENO); /* Duplicate used readend to file */ 
     close(file1);
#endif
     dup2 (file2, STDOUT_FILENO);/* Duplicate used writeend to file */ 
     
     //close(STDOUT_FILENO);
    
     close (fd[size-2][READ]); /* Close original used end */
     close (fd[size-2][WRITE]); /* Close original used end */
     close (fd[size-1][READ]); /* Close original used end */
     close (fd[size-1][WRITE]); /* Close original used end */
   

     close(file2);    
    //close(STDIN_FILENO);
    //close(STDOUT_FILENO);

        /* Execute writer program */ 
     DO_OR_DIE(execlp (commands[size-1], commands[size-1], params[size-1], NULL),
                 " The last command not works ");

    }
    


    for(int i=0;i<size;++i){
     close (fd[i][READ]); /* Close original used end */
     close (fd[i][WRITE]); /* Close original used end */
    }
    close(STDIN_FILENO);
    close(STDOUT_FILENO);

  return size;
}

  




int main(){
  char* argv [10] = {"cat","sort", "uniq"};  
  //char* pars [10] = {"input.txt", NULL, "-c"};  
  int n=3;
  //exec_seria(argv,pars,n); //  strace -f -o log.txt a.out
#if 1
  char cmd[255];
   char* token[40];
   char* coms[40];
   char* pars[40];
   char buf[20];
   memset (token, 0, sizeof(char*) * 40);
   memset (coms, 0, sizeof(char*) * 40);
   memset (pars, 0, sizeof(char*) * 40);
 
   int counter = 0;
   fgets (cmd, 255, stdin);
   //fscanf(stdin, "%[^\t]%s", cmd);
 //  printf("line %s", cmd); 

   int size = 0;
   for(int i=0;i<strlen(cmd);++i){
      if(cmd[i]=='|'){
          buf[size] = '\0';
          token[counter] = (char*) malloc(size);
          strcpy(token[counter],buf);
          size=0;
          counter++;  
      }
      else{
       buf[size++] = cmd[i];
    }
   }   

 buf[size] = '\0';
 token[counter] = (char*) malloc(size+1);
 strcpy(token[counter],buf);
 token[counter++][size]= '\0'; 
 
 printf("c=%d %d",counter,size);
for(int i=0;i<counter;++i){
  printf("\ni=%d %s (%zu)",i,token[i],strlen(token[i]));
  
  int start=0;
  while(isspace(token[i][start])) start++;

  int alp=start;
  while(isalnum(token[i][alp])) alp++;

  

  int end = strlen(token[i])-1;
  while(!isalnum(token[i][end])) end--;
  //end++;
  coms[i] = (char*) malloc(alp-start+1);


  for(int m=0;m<alp-start;++m){
   coms[i][m] = token[i][m+start];
  }
  coms[i][alp-start] ='\0';

  pars[i] = NULL;// =(char*) malloc(strlen(coms[i]));
  //strcpy(pars[i],coms[i]);
  printf("\n before %d %d %d",start, alp, end);
  if(end>alp){
  printf("\n afterst= %d %d %d",start, alp, end);
  pars[i] = (char*) malloc(end-alp+1);
  for(int m=alp+1;m<=end;++m){
   pars[i][m-alp-1] = token[i][m];
  }
  pars[i][end-alp] ='\0';
 }

  printf("com= %s (%zu)\n",coms[i],strlen(coms[i]));
  if(pars[i]) printf(" p=%s (%zu)\n",pars[i],strlen(pars[i]));

}


 exec_seria(coms,pars, counter);

for(int i=0;i<counter;++i){
   free(token[i]);
   free(coms[i]);
   if(pars[i])free(pars[i]);
}
#endif

    return 0;

}
