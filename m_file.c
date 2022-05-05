
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/ipc.h>

//#include <sys/shm.h> schmat?
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "m_file.h"

struct mon_message *createMessage(long type, char* text, int size_text)
{
    struct mon_message * s =
        malloc( sizeof(*s) + sizeof(char) * strlen(text));
  
    s->type = type;
    //s->name_len = size_text;
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
                    
    int fd;
    char* message;  /* mmapped array of chars */

    /* Open a file for writing.
     *  - Creating the file if it doesn't exist.
     *  - Truncating it to 0 size if it already exists. (not really needed)
     *
     * Note: "O_WRONLY" mode is not sufficient when mmaping.
     */
    fd = open(nom, options, mode);
    if (fd == -1) {
      perror("Error opening file for writing");
      exit(EXIT_FAILURE);
    }

    /* Stretch the file size to the size of the (mmapped) array 
     */
     
    size_t size_mes = sizeof(struct mon_message) + len_max;
    size_t filesize = nb_msg * size_mes;
     
    int result = lseek(fd, filesize, SEEK_SET);
    if (result == -1) {
      close(fd);
      perror("Error calling lseek() to 'stretch' the file");
      exit(EXIT_FAILURE);
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
      exit(EXIT_FAILURE);
    }

    /* Now the file is ready to be mmapped.
     */
    message = mmap(0, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (message == MAP_FAILED) {
      close(fd);
      perror("Error mmapping the file");
      exit(EXIT_FAILURE);
    }

         
    MESSAGE* res = (MESSAGE*) malloc(sizeof(MESSAGE));
    res->type = mode;
    res->maxLen = len_max;
    res->maxCap = nb_msg;
                    
    res->ptr = message;
    res->first = -1;
    res->last = -1;

    close(fd);
    return res;                                     
}



int m_deconnexion(MESSAGE *file){
  
  free(file);
  return 0;
}

int m_destruction(const char *nom){

    int fd = open(nom, O_RDONLY);
    if (fd == -1) {
      perror("Error opening file for writing");
      exit(EXIT_FAILURE);
    }
    
    struct stat st;
    stat(nom, &st);
    size_t filesize = st.st_size;
    
    MESSAGE* map = mmap(0, filesize, PROT_READ, MAP_SHARED, fd, 0);
    
    if (map == MAP_FAILED) {
      close(fd);
      perror("Error mmapping the file");
      exit(EXIT_FAILURE);
    }
     

    if (munmap(map, filesize) == -1) {
      perror("Error un-mmapping the file");
    }
    close(fd);
    
    return 0;
}



 int m_envoi(MESSAGE *file, const void *msg, size_t len, int msgflag){
   

   
   mon_message * mon_mes = (mon_message*) msg;
   printf("mes %s\n", mon_mes->mtext);
//   int* dig = (int*) (mon_mes->mtext);  
 //  printf("%ld (%d %d)",mon_mes->type, dig[0],dig[1]);
 
   if(file->first == file->last && file->last>=0) {
       errno = EMSGSIZE;
       return -1; 
   }
   
   if(len > file->maxLen){
      printf("Length wrong len=%zu ,maxLen=%zu", len, file->maxLen); 
      return -1;    
   }
   
   if(file->first ==-1){
       file->first = 0;
       file->last = 0;
   }
     
   
   //struct mon_message * message = createMessage(msgflag, NULL, file->maxLen);
   int size_message = sizeof(mon_message) + file->maxLen;   
    
   char* used_memory = (char*) (file->ptr + file->last * size_message);
   //memmove( used_memory, mon_mes->type, sizeof(mon_mes->type));
   memmove(used_memory, msg, len + sizeof(mon_message));
   
   struct mon_message *mon_mes2 = (mon_message*) used_memory;
     
   //int* dig2 = (int*) (mon_mes2->mtext);  
   //printf("\nM(%d %d)\n", dig2[0],dig2[1]);
    printf("Mes %s\n", mon_mes2->mtext);

   file->last++;
   if( file->last>=file->maxCap){
       file->last = 0;
   }
   
   return 0;
 }
 
ssize_t m_reception(MESSAGE *file, void *msg, size_t len, long type, int flags){
 
   if (len < file->maxLen){
       printf("len ");
      // errno = EMSGSIZE;
      // return -1;
   }
   
    if(file->first == -1 && file->last==-1){
        //????? if empy list
        errno=EAGAIN;
        return -2;
    }
    
   int size_message = sizeof(mon_message) + file->maxLen;   
   printf("ff %d", file->first); 

   
   if(type==0){
       
      void* used_memory = (void*) (file->ptr + file->first * size_message);
      memmove(msg, used_memory, len + sizeof(struct mon_message));
      file->first++;
      return 0;
   }
  /* else if(type>0){
       
      struct mon_message mm;
      bool found = false; 
      int i = file->first;
      void* used_memory = (void*) (file->ptr + file->first);
      
      if(file->ptr[i] == type){
          
          memmove(msg, used_memory, len + sizeof(struct mon_message));
           // delete meassage???
           
          file->first++;
          return 0;
      }
      
      
      while(i!=file->last){
          int i_next = i + 1;
          if(i_next == file->maxCap){
             i = 0; 
          }   
          
      
          if(file->ptr[i].type == type){
                   void* used_memory = (void*) (file->ptr + i_next);
                   memmove(&msg, used_memory, len);
                   
                   found = true;
                   
          }      
          memmove(used_memory, &mm, len);
          if(found){
              return 0;
          }
          memmove(&mm, used_memory, len);
          i = i_next;
                 
      }
      //printf("We havn't found type %ld - WTF??",type) ;
      errno=EAGAIN;
      return -1; 
   }
   
   struct mon_message mm;
      bool found = false; 
      int i = file->first;
      void* used_memory = (void*) (file->ptr + file->first);
      
      if(file->ptr[i].type == type){
          
          memmove(msg, used_memory, len);
           // delete meassage???
           
          file->first++;
          return 0;
      }
      
      
      while(i!=file->last){
          int i_next = i + 1;
          if(i_next == file->maxCap){
             i = 0; 
          }   
          
      
          if(file->ptr[i].type <= type){
                   void* used_memory = (void*) (file->ptr + i_next);
                   memmove(&msg, used_memory, len);
                   found = true;                   
          }      
          memmove(used_memory,&mm, len);
          if(found){
              return 0;
          }
          memmove(&mm, used_memory, len);
          i = i_next;
                 
      }*/
      
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

void printMessage(char* m, int sz){
    
    mon_message * mes = (mon_message *) m;
    //m->text = (char*)(m + sizeof(mon_message));
    
    printf("Message %ld: %s;\t",mes->type, mes->mtext);
    
    int* dig2 = (int*) (mes->mtext);  
    printf("\nRead(%d %d)\n", dig2[0],dig2[1]);
}

void printList(MESSAGE* mes){
    printf("print List (%u) Size:%zu, len:%zu, f/l %d -%d:\n",
             mes->type,mes->maxCap, mes->maxLen, mes->first, mes->last);

    int i = mes->first;
    int size_message = sizeof(struct mon_message) + mes->maxLen;
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
