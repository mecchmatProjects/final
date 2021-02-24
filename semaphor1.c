#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int main(){
    char fname[] = "1.txt" ;// "/tmp/sem.temp"
    key_t key = ftok(fname, 1);
    int semid = semget(key, 16, IPC_CREAT | 0666); // sid
    if(semid==-1){
     fprintf(stderr,"Cant get semaphor %s",fname);

    }

    struct sembuf sops[16];
    for (int i = 0; i < 16; i++) {
        sops[i].sem_num = i;
        sops[i].sem_op = i;
        sops[i].sem_flg = 0;
    }
    semop(semid, sops, 16); // ipcs -s , ipcs -s -i <number of semaphor>32769  - to watch
    perror("Errno after semop: ");
    return 0;
}
