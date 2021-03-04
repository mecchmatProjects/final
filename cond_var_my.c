
#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>

pthread_t tid[5];
int counter;
pthread_mutex_t lock;

pthread_cond_t condition;


pthread_barrier_t   barrier; // the barrier synchronization object

static int comm = 0;

void* doSomeThing(void *arg)
{

    unsigned long i = 0;
    sleep(1);
    pthread_mutex_lock(&lock);
    
    sleep(1);
    counter++;
    int local = counter;

    pthread_mutex_unlock(&lock);

   while(1){
    if(comm>=80) {
       pthread_cond_signal(&condition);
       break;
    }
    pthread_mutex_lock(&lock);


    printf("\n Job %d started\n", local);
    comm++;

    printf("\n Job %d %d finished\n", local, comm);

    pthread_mutex_unlock(&lock);
    sleep(0.01 * (local+1) * (comm+1));

   }
     
    printf("\n Thread %d finished\n", local);
    return NULL;
}



void* doAnotherThing(void *arg)
{

    unsigned long i = 0;
    sleep(1);
    pthread_mutex_lock(&lock);
    
    sleep(1);
    counter++;
    int local = counter;

    while (comm < 70)	{

	pthread_cond_wait(&condition, &lock);
		}

    comm *=10;   
    printf("\n Job %d %d done\n", local, comm);

    pthread_mutex_unlock(&lock);

   
     
    printf("\n Thread %d finished\n", local);
    return NULL;
}


void * thread3 (void *not_used)
{
    
    // do the computation
    // let's just do a sleep here...
    sleep (5);
    pthread_barrier_wait (&barrier);
    // after this point, all three threads have completed.
    
    printf ("barrier in thread3() done");
}

void * thread4 (void *not_used)
{
    
    // do the computation
    // let's just do a sleep here...
    sleep (15);
    pthread_barrier_wait (&barrier);
    // after this point, all three threads have completed.
    
    printf ("barrier in thread4() done");
}


int main(void)
{
    int i = 0;
    int err;

    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }

	pthread_cond_init(&condition, NULL);


    while(i < 2)
    {
        err = pthread_create(&(tid[i]), NULL, &doSomeThing, NULL);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        i++;
    }

    pthread_create(&(tid[2]), NULL, &doAnotherThing, NULL);


    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    pthread_join(tid[2], NULL);

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&condition);

    pthread_barrier_init (&barrier, NULL, 3);

   // start up two threads, thread1 and thread2
    pthread_create (&(tid[3]), NULL, thread3, NULL);
    pthread_create (&(tid[4]), NULL, thread4, NULL);

    pthread_barrier_wait (&barrier);

    return 0;
}
