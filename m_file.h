#ifndef __M_FILE_H__
#define __M_FILE_H__
/*
 Header file for the function in the project
*/

//#include <unistd.h>



/*
  Structure of the mesage   
*/
typedef struct mon_message{
   long type;  //  the type of the message
   char mtext[]; // the message array
}mon_message;


/*
The MESSAGE type is a structure that contains at least the following elements:
1. the type of opening of the message queue (read, write, read and write);
this information is necessary for m_sending and m_receiving operations; 
it must verify that the operation is authorized, for example m_envoi must fail if the
file was opened for reading only;
2. the pointer to the shared memory which contains the queue.
*/

typedef struct MESSAGE{
   mode_t type;
   
   char* ptr;
   
   size_t maxLen;
   size_t maxCap;
      
   int first;
   int last;
   
    
} MESSAGE;



/*
   m_connexion - connects to an existing message queue, or 
   create a new message queue and connect to it:
   
 - nom : the name of the queue or NULL for an anonymous queue;
 - options:  
     If options does not contain O_CREAT, then the m_connection function will only have the two
     parameters name and options, i.e. m_connection is a variable number function
     arguments (either 2 or 5)
       Constants:
     – O_RDWR, O_RDONLY, O_WRONLY, with the same meaning as for open;
        exactly one of these constants must be specified;
     – O_CREAT to request the creation of the queue;
     – O_EXCL, in combination with O_CREAT, indicates that the queue must be created only if
      she does not exist ; if the queue already exists, m_connection should fail.
 - nb_msg: the capacity of the queue, i.e. the (minimum) number of messages that can be
   store before the queue is full 
 - len_max: the maximum length of a message
 - mode:  gives the permissions granted for the new message queue ("OR" bitwise)
     according to man 2 chmod
 

 Return: 
    pointer to a MESSAGE type object that identifies the queue of
    messages and will be used by other functions
    On failure, m_connection returns NULL.
*/

extern MESSAGE *m_connexion( const char *nom, int options, 
                    size_t nb_msg, size_t len_max, mode_t mode);
                    

/*
m_deconnexion - disconnects the process from the message queue; 
    it is not destroyed, but file becomes unusable.
    
Return:
  0 if OK and −1 in case of error.
*/
extern int m_deconnexion(MESSAGE *file);

/*
  m_destruction - deletes the message queue; 
  
  effective deletion only takes place when the last connected process disconnects from the queue;
   on the other hand, once m_destruction executed by a process, all subsequent m_connection attempts fail.
Return:
  0 if OK, −1 if failure.
*/
extern int m_destruction(const char *nom);



/*
  m_envoi - sends the message to the queue; 
 - file: MESSAGE pointer
 - msg: is a pointer to the message to send; 
 - len: the length of the message in bytes; 
   (the length of the mtext field of struct my_message)
- msgflag:  parameter, that can take two values:
  0: the calling process is blocked until the message is sent
      that can happen when there is no more room in the queue;
  O_NONBLOCK: if there is no space in the queue, then the call 
   returns 1 and errno == EAGAIN.
Return:
    0 when the sending succeeds,
    1 otherwise.
If the length of the message is greater than the maximum length supported by the queue,
the function immediately returns 1 and puts errno = EMSGSIZE 
*/
extern int m_envoi(MESSAGE *file, const void *msg, size_t len, int msgflag);



/*
   m_reception - Reads the first suitable message on the queue, copies it to the address msg and
remove from queue.
 -msg : the address to which the function should copy the read message; 
  this message will be removed from the queue;
 -len : the length (in bytes) of memory at address msg; 
    if len is lower to the length of the message to read,
      m_reception fails and returns 1 and errno == EMSGSIZE.
 -type: specifies the request:
    if type == 0: read the first message in the queue;
    if type > 0: read the first message whose type is type; 
      severalprocesses can use the same message queue, 
      for example each process can use its pid as a type value;
    if type < 0: treat the queue as a priority queue; in this case, this means:
     read the first message whose type is less than or equal to the absolute value of type.
 - flags : either the value 0 or O_NONBLOCK; 
    if flags == 0, the call is blocking until the read succeeds; 
    if flags == O_NONBLOCK, and if there is no message of the requested type in the queue,
    return 1 and errno==EAGAIN.

A message queue is a FIFO queue: if we write the messages a, b, c in this order, and
if we read with type==0, then successive reads return a, b, c in the same order.

Return:
  the number of bytes of the message read, or -1 in case of failure

*/
extern ssize_t m_reception(MESSAGE *file, void *msg, size_t len, long type, int flags);

/*
Return:
 maximum size of a message
*/
extern size_t m_message_len(MESSAGE * file);
/*
Return:
  the maximum number number of messages in the queue
*/
extern size_t m_capacite(MESSAGE * file);

/*
Return:
  the number of messages currently in the queue
*/
extern size_t m_nb(MESSAGE * file);

//DEBUG
extern void printMessage(char* m, int sz);

extern void printList(MESSAGE* mes);



#endif //end of __M_FILE_H__
