#include <iostream>
#include <cstdio>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

union semnum {
  int val;
  struct semid_ds *buf;
  unsigned short *array;
};

int main() {
  constexpr size_t sem_q = 16;
  char fname[] ="1.txt";
  key_t key = ftok(fname, 1);
  int sem_id =semget(key, 0, 0);

  if (sem_id == -1){
     sem_id = semget(key, sem_q, IPC_CREAT | 0666); //= )   
  }
 else{
      fprintf(stderr,"Cant get semaphor %s %d: remove it",fname, sem_id);
      semctl(sem_id, 0, IPC_RMID);
      sem_id = semget(key, sem_q, IPC_CREAT | 0666); 
  }

  //auto sem_id = semget(key, sem_q, IPC_CREAT | 0666); //= )


  for (size_t i = 0; i < sem_q; ++i) {
    semnum arg;
    arg.val = i;
    semctl(sem_id, i, SETVAL, arg);
  }

  return 0;
}
