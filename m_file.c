
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/ipc.h>

//#include <sys/shm.h> schmat?
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "m_file.h"

struct MessageText *createMessage(long type, char* text, int size_text)
{
    struct MessageText * s =
        malloc( sizeof(*s) + sizeof(char) * strlen(text));
  
    s->type = type;
    s->len = size_text;
    if(text){
        memmove(s->mtext, text, size_text);
    }
    else{
        for (int i = 0; i < size_text; ++i) {
          s->mtext[i] = 0;
        } 
    }
   return s;
}



MESSAGE *m_connexion( const char *nom, int options, 
                    size_t nb_msg, size_t len_max, mode_t mode ){
   
     
    size_t size_mes = sizeof(MessageText) + len_max;
    size_t filesize = nb_msg * size_mes;
    char* addr;  /* mmapped array of chars */
                 
                        
    if(nom){  
              
        /* Open a file for writing.
        *  - Creating the file if it doesn't exist.
        *  - Truncating it to 0 size if it already exists. (not really needed)
        *
        * Note: "O_WRONLY" mode is not sufficient when mmaping.
        */
        int fd = open(nom, options, mode);
        if (fd == -1) {
            perror("Error opening file for writing");
            //exit(EXIT_FAILURE);
            return NULL;
        }

        /* Stretch the file size to the size of the (mmapped) array */
        int result = lseek(fd, filesize, SEEK_SET);
        if (result == -1) {
            close(fd);
            perror("Error calling lseek() to 'stretch' the file");
            //exit(EXIT_FAILURE);
            return NULL;
        }
    
    /* Something needs to be written at the end of the file to
     * have the file actually have the new size.
     * Just writing an empty string at the current file position will do.
     *
     * Note:
     *  - The current position in the file is at the end of the stretched 
     *    file due to the call to lseek().
     *  - An empty string is actually a single '\0' character, so a zero-byte
     *    will be written at the last byte of the file.
     */
    result = write(fd, "", 1);
    if (result != 1) {
      close(fd);
      perror("Error writing last byte of the file");
      //exit(EXIT_FAILURE);
      return NULL;
    }

    /* Now the file is ready to be mmapped.
     */
        addr = mmap(0, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (addr == MAP_FAILED) {
            close(fd);
            perror("Error mmapping the file");
            //exit(EXIT_FAILURE);
            return NULL;
        }
        close(fd);

    }
    else{
         addr = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);     
         if (addr == MAP_FAILED) {     
            perror("Error mmapping the file");   
            //exit(EXIT_FAILURE);
           return NULL;
     }      
    }
         
    MESSAGE* res = (MESSAGE*) malloc(sizeof(MESSAGE));
    res->type = mode;
    res->maxLen = len_max;
    res->maxCap = nb_msg;
                    
    res->ptr = addr;
    res->first = -1;
    res->last = -1;

    return res;                                     
}



int m_deconnexion(MESSAGE *file){
  
  size_t size_mes = sizeof(MessageText) + file->maxLen;
  size_t filesize = file->maxCap * size_mes;
   
  if (munmap(file->ptr, filesize) == -1) {
      perror("Error un-mmapping the file");
    } 
   
  free(file);
  return 0;
}

int m_destruction(const char *nom){

    if(!nom) return -1;
    
    int status = remove(nom); //unlink
    return status;
}



 int m_envoi(MESSAGE *file, const void *msg, size_t len, int msgflag){
     
   assert(file && "no file");
   assert(msg && "no data");
   assert((msgflag==0 || msgflag==O_NONBLOCK) && "Wrong flag in send");
   
   if(file->first == file->last && file->last>=0 && msgflag==O_NONBLOCK) {
       errno = EMSGSIZE;
       return -1; 
   }
   
   if(len > file->maxLen){
      printf("Length wrong len=%zu ,maxLen=%zu", len, file->maxLen); 
      return -1;    
   }  
   printf("Length good"); 


   struct mon_message * pMes = (struct mon_message*) msg;  
     
   MessageText * mon_mes = createMessage(pMes->type, pMes->mtext, len);
   
   // debug
   printf("mes %s\n", mon_mes->mtext);
   int* dig = (int*) (mon_mes->mtext);  
   printf("%ld (%d %d)",mon_mes->type, dig[0],dig[1]);
  
   
   if(file->first ==-1){
       file->first = 0;
       file->last = 0;
   }
     
   
   //struct mon_message * message = createMessage(msgflag, NULL, file->maxLen);
   
   pthread_mutex_lock(&file->lock);
   int size_message = sizeof(MessageText) + file->maxLen * sizeof(char);   
    
   char* used_memory = (char*) (file->ptr + file->last * size_message);
   //memmove( used_memory, mon_mes->type, sizeof(mon_mes->type));
   memmove(used_memory, mon_mes, sizeof(MessageText) + len);
   file->last++;
   if( file->last>=file->maxCap){
       file->last = 0;
   }
   pthread_mutex_unlock(&file->lock);
   //Debug
   MessageText *mon_mes2 = (MessageText*) used_memory;
   int* dig2 = (int*) (mon_mes2->mtext);  
   printf("\nM(%d %d)\n", dig2[0],dig2[1]);
   printf("Mes %s\n", mon_mes2->mtext);

   
   
   return 0;
 }
 
ssize_t m_reception(MESSAGE *file, void *msg, size_t len, long type, int flags){
 
    assert(file && "no file");
    assert(msg && "no data");
    assert((flags==0 || flags==O_NONBLOCK) && "Wrong flag in Reception");

 
    
   int size_message = sizeof(MessageText) + file->maxLen;   
   printf("ff %d", file->first); 

   
   if(type==0){
       
      void* used_memory = (void*) (file->ptr + file->first * size_message);
      MessageText *rec_msg = (MessageText*) used_memory;
      
      if (len < rec_msg->len){
        printf("len  of msg = %zu, reserved=%zu", len, rec_msg->len);
        errno = EMSGSIZE;
        return -1;
      }
      
      pthread_mutex_lock(&file->lock);
      struct mon_message * dec_msg = (struct mon_message *) msg;
      if(!dec_msg){
          printf("Eror having %zu memory inside recv", rec_msg->len);
       }
      dec_msg->type = rec_msg->type;
      //printf("L=%zu, M=%zu", rec_msg->len, len);
      //memmove(dec_msg.mtext, rec_msg->mtext, rec_msg->len);s
      for(int i=0;i<rec_msg->len;i++){
          dec_msg->mtext[i] = rec_msg->mtext[i];
      }
      //memmove(msg, dec_msg, sizeof(struct mon_message) + rec_msg->len);/**/
      file->first++;
      if( file->last>=file->maxCap){
          file->last = 0;
      }
      pthread_mutex_unlock(&file->lock);
     // free(dec_msg);
      return 0;
   }
   else if(type>0){
       
      void* used_memory = (void*) (file->ptr + file->first * size_message);
      bool found = false; 
      int i = file->first;
      MessageText *rec_msg = (MessageText*) used_memory;
      
      // cycle:      
      while(i!=file->last || !flags){
         

          if(rec_msg->type == type){
              
            pthread_mutex_lock(&file->lock); 
            if (len < rec_msg->len){
                    printf("len  of msg = %zu, reserved=%zu", len, rec_msg->len);
                errno = EMSGSIZE;
                return -1;
                }

            struct mon_message * dec_msg = (struct mon_message *) msg;
            if(!dec_msg){
            printf("Eror having %zu memory inside recv", rec_msg->len);
          }
          dec_msg->type = rec_msg->type;
      //printf("L=%zu, M=%zu", rec_msg->len, len);
      //memmove(dec_msg.mtext, rec_msg->mtext, rec_msg->len);s
          for(int i=0;i<rec_msg->len;i++){
            dec_msg->mtext[i] = rec_msg->mtext[i];
          }
      //memmove(msg, dec_msg, sizeof(struct mon_message) + rec_msg->len);/**/
          file->first++;
          if(file->first>=file->maxCap){
              file->first = 0;
          }
          pthread_mutex_unlock(&file->lock);  
          
          return 0;
         }
          
          i++;        
          if(i == file->maxCap){
             i = 0; 
          }   
          
      
      }
      
    
        printf("We havn't found type %ld - WTF??",type) ;
        errno=EAGAIN;
        return -1; 
    
   }
   
   void* used_memory = (void*) (file->ptr + file->first * size_message);
      bool found = false; 
      int i = file->first;
      MessageText *rec_msg = (MessageText*) used_memory;
      
      // cycle:      
      while(i!=file->last || !flags){
          
          if(rec_msg->type <= type){
                pthread_mutex_lock(&file->lock);

                if (len < rec_msg->len){
                printf("len  of msg = %zu, reserved=%zu", len, rec_msg->len);
                errno = EMSGSIZE;
                return -1;
                }

          struct mon_message * dec_msg = (struct mon_message *) msg;
          if(!dec_msg){
            printf("Eror having %zu memory inside recv", rec_msg->len);
          }
          dec_msg->type = rec_msg->type;
      //printf("L=%zu, M=%zu", rec_msg->len, len);
      //memmove(dec_msg.mtext, rec_msg->mtext, rec_msg->len);s
          for(int i=0;i<rec_msg->len;i++){
            dec_msg->mtext[i] = rec_msg->mtext[i];
          }
      //memmove(msg, dec_msg, sizeof(struct mon_message) + rec_msg->len);/**/
          file->first++;
          if(file->first>=file->maxCap){
              file->first = 0;
          }
          pthread_mutex_unlock(&file->lock);  
          
          return 0;
         }
          
          i++;        
          if(i == file->maxCap){
             i = 0; 
          }   
          
      
      }
          
       
  return 0;     
}
 
 
size_t m_message_len(MESSAGE * file){
    return file->maxLen; 
}

size_t m_capacite(MESSAGE * file){
    return file->maxCap;
}

size_t m_nb(MESSAGE * file){
    int dif = file->last - file->first;
    return dif>0?dif:(dif + file->maxCap);
}

void printMessage2(struct mon_message* m){
    
    struct mon_message * mes = (struct mon_message*) m;
    //m->text = (char*)(m + sizeof(mon_message));
    
    printf("Message %ld: %s;\t",mes->type, mes->mtext);
    
    int* dig2 = (int*) (mes->mtext);  
    printf("\nRead(%d %d)\n", dig2[0],dig2[1]);
}

void printMessage(char* m, int sz){
    
    MessageText * mes = (MessageText *) m;
    //m->text = (char*)(m + sizeof(mon_message));
    
    printf("Message %ld: %s;\t",mes->type, mes->mtext);
    
    int* dig2 = (int*) (mes->mtext);  
    printf("\nRead(%d %d)\n", dig2[0],dig2[1]);
}

void printList(MESSAGE* mes){
    printf("print List (%u) Size:%zu, len:%zu, f/l %d -%d:\n",
             mes->type,mes->maxCap, mes->maxLen, mes->first, mes->last);

    int i = mes->first;
    int size_message = sizeof(MessageText) + mes->maxLen;
    char* mes_ptr = (mes->ptr + i * size_message);
    while(i!=mes->last){
        printf("\t %d-th: ",i);
        printMessage(mes_ptr,size_message);
        i++;
        if(i>=mes->maxCap){
            i = 0;
            mes_ptr = mes->ptr;
        }
        else{
            mes_ptr += size_message;
        }
    }
}
