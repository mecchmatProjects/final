#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <stddef.h>

struct my_msgbuf {
    long mtype;
    char mtext[80];
};



int main(){

 char fname[] =  "result.out"; // "/tmp/msg.temp\0";
 char fout[] = "1.txt";// "/home/box/message.txt";

 key_t key;

 int msqid; // = msgget(key, 0666 | IPC_CREAT);

 if ((key = ftok(fname, 1)) == -1) {  /* same key as msg_send.c */
        perror("ftok");
        exit(1);
    }

    if ((msqid = msgget(key, 0666 | IPC_CREAT)) == -1) { /* connect to the queue */
        perror("msgget");
        exit(1);
    }
    


 struct my_msgbuf buf;
 
 FILE * fp;


 //while(1) { 
        fp = fopen(fout,"wt"); 
        if (msgrcv(msqid, &buf, sizeof buf.mtext, 0, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }
       printf("%s\n",buf.mtext);
       fprintf(fp,"%s", buf.mtext);
       fclose(fp);
  //  }

 if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(1);
    }

}
