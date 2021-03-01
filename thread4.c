
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details. 
#include <pthread.h> 
#include <fcntl.h>
  
// A normal C function that is executed as a thread  
// when its name is specified in pthread_create() 
void *myThreadFun(void *vargp) 
{ 
    //sleep(1); 
    //char buf[] ="sdffsd";
    printf("Printing from Thread %s \n",(char*)vargp); 


    char fname[] = "/home/box/main.pid";// "2.txt";//"/home/box/main.pid"
    /*FILE* fp;

    freopen(fname,"wt");
    //fprintf(fp,"%d",123);
    fclose(fp);*/

    
    int fd = open(fname, O_WRONLY | O_CREAT|O_TRUNC, S_IRUSR |S_IWUSR);
    write(fd,(char*)vargp,strlen((char*)vargp));
    close(fd);

    pthread_exit(NULL);
    return NULL; 
} 
   
int main() 
{ 
    pthread_t thread_id; 
    printf("Before Thread\n"); 
    int tid = pthread_self();
    char buf[20];
    snprintf(buf,20,"%d",tid);
    printf("Before Thread %s %d\n",buf, thread_id); 
    pthread_create(&thread_id, NULL, myThreadFun, buf); 

    pthread_join(thread_id, NULL); 
    printf("After Thread\n"); 
    exit(0); 
}

