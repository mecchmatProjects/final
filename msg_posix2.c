#include <signal.h>
#include <unistd.h>
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>


#define MSG_SIZE       255

// This handler will be called when the queue 
// becomes non-empty.

void handler (int sig_num) {
    printf ("Received sig %d.\n", sig_num);
}
    
void main () {

  struct mq_attr attr, old_attr;   // To store queue attributes
  struct sigevent sigevent;        // For notification
  mqd_t mqdes, mqdes2;             // Message queue descriptors
  char buf[MSG_SIZE];              // A good-sized buffer
  unsigned int prio;               // Priority 

  char fname[] = "/home/box/message.txt";

 // First we need to set up the attribute structure
  attr.mq_maxmsg = 300;
  attr.mq_msgsize = MSG_SIZE;
  attr.mq_flags = 0;
    
  // Open a queue with the attribute structure
  mqdes = mq_open ("/test.mq", O_RDWR | O_CREAT, 
                   0664, 0);

  // This will now be a temporary queue...as soon as it's closed,
  // it will be removed
  mq_unlink ("/test.mq");

  // Get the attributes for Sideshow Bob
  mq_getattr (mqdes, &attr);
  printf ("%ld messages are currently on the queue.\n", 
          attr.mq_curmsgs);

  //if (attr.mq_curmsgs != 0) {
  while(1){  
    // There are some messages on this queue....eat em
    
    // First set the queue to not block any calls    
   // attr.mq_flags = O_NONBLOCK;
   // mq_setattr (mqdes, &attr, &old_attr);    
        
    fp = fopen(fname,"wt"); 
     int rc = mq_receive (mqdes, &buf[0], MSG_SIZE, &prio);
        if ( rc!= -1){ 
             printf ("Received a message with priority %d.\n", prio);
            
        }
        else{
           continue;
        }
            
       printf("%s\n",buf.mtext);
       buf.mtext[rc] ='\0';
       fprintf(fp,"%s\n", buf.mtext);
       fclose(fp);
        
    
        
    // Now restore the attributes
   // mq_setattr (mqdes, &old_attr, 0);            
  }
    
    

  // Close all open message queue descriptors    
  mq_close (mqdes);
  mq_unlink ("/test.mq");
}


