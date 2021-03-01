#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>

char *pid_file = "pthread.pid";
// char *pid_file = "/home/box/main.pid";

pthread_mutex_t mutex;
pthread_spinlock_t slock;
pthread_rwlock_t rwlock;

void save_pid( pid_t pid, char *filename )
{
    FILE* f;

    // Open file
    f = fopen( filename, "w+" );
    if( !f )
    {
        perror( NULL );
        printf( "Ошибка открытия файла: %s\n", filename );
        return;
    }

    // Save pid into file
    fprintf( f, "%d\n", pid );

    // Close file
    fclose(f);
}

void cleanup_handler( void *arg )
{
    printf("Thread canceled.\n");
}

void *pthread_func_1( void *p )
{
    printf("Mutex thread started.\n");
    pthread_cleanup_push( cleanup_handler, NULL );

    pthread_mutex_lock( &mutex );
    printf("Mutex unloked!!!\n");
    pthread_mutex_unlock( &mutex );

    pthread_cleanup_pop(1);
    pthread_exit((void *)0);
}

void *pthread_func_2( void *p )
{
    printf("Spin thread started.\n");
    pthread_cleanup_push( cleanup_handler, NULL );

    pthread_spin_lock( &slock );
    printf("Spin unloked!!!\n");
    pthread_spin_unlock( &slock );

    pthread_cleanup_pop(1);
    pthread_exit((void *)0);
}

void *pthread_func_3( void *p )
{
    printf("Rw read thread started.\n");
    pthread_cleanup_push( cleanup_handler, NULL );

    pthread_rwlock_rdlock( &rwlock );
    printf("Rw read unloked!!!\n");
    pthread_rwlock_unlock( &rwlock );

    pthread_cleanup_pop(1);
    pthread_exit((void *)0);
}

void *pthread_func_4( void *p )
{
    printf("Rw write thread started.\n");
    pthread_cleanup_push( cleanup_handler, NULL );

    pthread_rwlock_wrlock( &rwlock );
    printf("Rw write unloked!!!\n");
    pthread_rwlock_unlock( &rwlock );

    pthread_cleanup_pop(1);
    pthread_exit((void *)0);
}


int main( int argc, char **argv )
{
    int result;
    pid_t pid;

    // Get process id
    pid = getpid();
    // save pid
    save_pid( pid, pid_file );
    printf("pid %d\n", pid );

    // Mutex init and lock mutex
    pthread_mutex_init( &mutex, NULL );
    pthread_mutex_lock( &mutex );
    // Spin lock init and lock
    pthread_spin_init( &slock, PTHREAD_PROCESS_PRIVATE );
    pthread_spin_lock( &slock );
    // rwlock init and lock to read and write
    pthread_rwlock_init( &rwlock, PTHREAD_PROCESS_PRIVATE );
    // pthread_rwlock_rdlock( &rwlock );
    pthread_rwlock_wrlock( &rwlock );

    // Create threads
    pthread_t thread_id1;
    pthread_create( &thread_id1, NULL, pthread_func_1, NULL );
    pthread_t thread_id2;
    pthread_create( &thread_id1, NULL, pthread_func_2, NULL );
    pthread_t thread_id3;
    pthread_create( &thread_id3, NULL, pthread_func_3, NULL );
    pthread_t thread_id4;
    pthread_create( &thread_id4, NULL, pthread_func_4, NULL );

    // Set signal handler
    sigset_t sigset;
    sigemptyset( &sigset );
    sigaddset( &sigset, SIGTERM );
    sigaddset( &sigset, SIGINT );
    sigprocmask(SIG_BLOCK, &sigset, NULL);

    int sig;
    sigwait( &sigset, &sig );

    // Unlock mutex
    pthread_mutex_unlock( &mutex );
    // Unlock spin
    pthread_spin_unlock( &slock );
    // Unlock rwlock
    pthread_rwlock_unlock( &rwlock );

    // join
    pthread_join( thread_id1, NULL );
    pthread_join( thread_id2, NULL );
    pthread_join( thread_id3, NULL );
    pthread_join( thread_id4, NULL );

    pthread_mutex_destroy( &mutex );
    pthread_spin_destroy( &slock );
    pthread_rwlock_destroy( &rwlock );

    return 0;
}
