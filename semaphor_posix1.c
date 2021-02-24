// C program to demonstrate working of Semaphores 
#include <stdio.h> 
#include <pthread.h> 

#include <semaphore.h>
#include <unistd.h> 
#include <fcntl.h>          /* O_CREAT, O_EXEC          */

int main() 
{ 

    char name[] = "/test.sem";
    int value = 66;
    sem_t *sem;
    /* initialize semaphores for shared processes */
    sem = sem_open (name, O_CREAT, 0644, value);
    if(!sem){
         printf ("semaphores not initialized.\n\n");
    }  
    /* name of semaphore is "pSem", semaphore is reached using this name */
    sem_unlink (name);      
    /* unlink prevents the semaphore existing forever */
    /* if a crash occurs during the execution         */
    printf ("semaphores initialized.\n\n");
} 

