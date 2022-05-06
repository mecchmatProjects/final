#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

#include "m_file.h"

//И будут ли как-то оцениваться собственные тесты

int main(int argc, char* argv[]){
    
        
  // given test         

  MESSAGE * file = m_connexion( "file_int", O_CREAT|O_RDWR, 
                    5, 8, 0666);


  int t[2] = {-12, 99}; /*valeurs à envoyer*/
  struct mon_message *m = malloc( sizeof( struct mon_message ) + sizeof( t ) );


   if( m == NULL ){
     /* traiter erreur de malloc */ 
     printf("Alloc error");
     return -1;  
    }
    m->type = (long) getpid(); /* comme type de message, on choisit l’identité * de l’expéditeur */
    memmove( m -> mtext, t, sizeof( t )) ; /* copier les deux int à envoyer */

    /* send in non-blocking mode */
   int i = m_envoi( file, m, sizeof( t ), O_NONBLOCK) ;
   if( i == 0 ){ /* message envoyé */ 
       printf("mesage is sent"); 
   }
   else if( i == -1 && errno == EAGAIN ){
      /* file pleine, essayer plus tard */
      printf(" file is full, emty it");
   }
   else{ /* erreur de la fonction */ 
     printf("error");
   }
   
  free(m);  
  printf("State of Int: Len(%zu), Cap(%zu), CurrentSize (%zu) \n",
          m_message_len(file),m_capacite(file), m_nb(file));
          
  printList(file);   
  
  int t1[] ={1};
  struct mon_message *m1 = malloc( sizeof( struct mon_message ) + sizeof( t1 ) );
  m1->type = (long) getpid(); /* comme type de message, on choisit l’identité * de l’expéditeur */
  memmove( m1 -> mtext, t1, sizeof( t1 )) ;
  /* send in non-blocking mode */
   i = m_envoi( file, m1, sizeof( t1 ), O_NONBLOCK) ;
   if( i == 0 ){ /* message envoyé */ 
       printf("mesage is sent"); 
   }
   else if( i == -1 && errno == EAGAIN ){
      /* file pleine, essayer plus tard */
      printf(" file is full, emty it");
   }
   else{ /* erreur de la fonction */ 
     printf("error");
   }
   
  free(m1);  
  printf("State of Int: Len(%zu), Cap(%zu), CurrentSize (%zu) \n",
          m_message_len(file),m_capacite(file), m_nb(file));
          
  printList(file);   
 
 
  struct mon_message *mes0 = malloc( sizeof( struct mon_message ) + sizeof(t));
  ssize_t rec0 = m_reception(file, mes0, sizeof(t), 0, O_NONBLOCK);

  if(rec0==0){
    printf("\nfirst: ");  
    printMessage2(mes0); 
  }
  free(mes0);
  
  printList(file);   

  struct mon_message *mes01 = malloc( sizeof( struct mon_message ) + sizeof(t) );

  ssize_t rec01 = m_reception(file, mes01, sizeof(t1), 0, O_NONBLOCK);

   
  if(rec01==0){
    printf("\nsecond: ");  
    printMessage2(mes01); 
  }
  else{
          printf("can't get second");  

    }
  free(mes01);
  
  printList(file);   
 


  int dic = m_deconnexion(file);
  if (dic!=0){ 
     printf("Not disconnected");
     exit(-1);
  }
  else{
     printf("Disconnected");      
  }  
 
  int destr = m_destruction("file_int");
     if (destr!=0){ 
     printf("Not destructed");
     exit(-1);
   }
   else{
     printf("Destructed");      
   }  

    
    
    

#if 1

  //basic test
  char* buf[] ={"mes1","messs2","me3", "mes4",};
  int buf_size = sizeof(buf)/sizeof(*buf);
  
  MESSAGE * MES_BASE = m_connexion( "MES_BASE", O_CREAT|O_RDWR, 
                    5, 8, 0666);
  
  printf("Initial state of MESSAGE: Len(%zu), Cap(%zu), CurrentSize (%zu)\n",
          m_message_len(MES_BASE),m_capacite(MES_BASE), m_nb(MES_BASE));
          
  for(int i=0;i<3;i++){
      struct mon_message *mes1 = malloc( sizeof( struct mon_message ) + strlen(buf[i])+1 );
      memmove( mes1 -> mtext, buf[i], strlen(buf[i])+1 ) ;
      if( mes1 == NULL ){
          printf("Alloc error");
          return -1;  
      }
      mes1->type = (long) getpid();
      
      int r = m_envoi(MES_BASE, mes1, strlen(buf[i]) + 1, O_NONBLOCK);
      if(r>=0){
         printf("send successful\n");
      }
      else{
        printf(" send unsuccessful\n");
      }
      
      free(mes1);
  }        
  printf("\n\n send 3 mes");
  printf("Next state of MESSAGE: Len(%zu), Cap(%zu), CurrentSize (%zu)\n",
          m_message_len(MES_BASE),m_capacite(MES_BASE), m_nb(MES_BASE));
          
  printList(MES_BASE);        
          
  for(int i=0;i<3;i++){
      struct mon_message *mes1 = malloc( sizeof( struct mon_message ) + strlen(buf[i])+1 );
      if( mes1 == NULL ){
          printf("Alloc error");
          return -1;  
      }
      mes1->type = (long) getpid();
      memmove( mes1 -> mtext, buf[i], strlen(buf[i])+1 ) ;
      int r = m_envoi(MES_BASE, mes1, strlen(buf[i])+1, O_NONBLOCK);
      if(r>=0){
         printf("send successful\n");
      }
      else{
        printf(" send unsuccessful\n");
      }
      
      free(mes1);
  }
  
  printf("\n\n send more 3 mes");
  printf("Next state of MESSAGE: Len(%zu), Cap(%zu), CurrentSize (%zu)\n",
          m_message_len(MES_BASE),m_capacite(MES_BASE), m_nb(MES_BASE));
  printList(MES_BASE);           
 
  struct mon_message* mes2;
  mes2 = (struct mon_message*) malloc( sizeof( struct mon_message ) + sizeof( strlen(buf[0])+1 ) );

  ssize_t rec = m_reception(MES_BASE, mes2,  strlen(buf[0])+1, 0, O_NONBLOCK);
  
  if(rec<0){
     printf("No msg received %zd\n",rec);
  }
  else{
     printf("Msg received %zd\n",rec);
     printf("Msg=%s\n", mes2->mtext);  
  }
  
  printf("\n\n recieve 1 mes");

   printf("Next state of MESSAGE: Len(%zu), Cap(%zu), CurrentSize (%zu)\n",
          m_message_len(MES_BASE),m_capacite(MES_BASE), m_nb(MES_BASE));
  
   printList(MES_BASE);   
  
   free(mes2);
  printf("\n\n recieve 5 mes");
  
   for(int i=0;i<5;i++){
     struct mon_message *mes3 = malloc( sizeof( struct mon_message ) + sizeof( strlen(buf[0])+1 ) );

     ssize_t rec = m_reception(MES_BASE, mes3,  strlen(buf[0])+1, 0, O_NONBLOCK);
  
     if(rec<0){
       printf("No msg received %zd",rec);
     }
     else{
       printf("Msg received %zd\n",rec);
       printf("Msg=%s\n", mes3->mtext);  
     }
  
     printf("Next state of MESSAGE: Len(%zu), Cap(%zu), CurrentSize (%zu)",
          m_message_len(MES_BASE),m_capacite(MES_BASE), m_nb(MES_BASE));
  
     free(mes3); 
   }
   printList(MES_BASE);   
  
   int dic1 = m_deconnexion(MES_BASE);
   if (dic1!=0){ 
     printf("Not disconnected");
     exit(-1);
   }
   else{
     printf("Disconnected");      
   }  
   
   int destr1 = m_destruction("MES_BASE");
   if (destr1!=0){ 
     printf("Not destructed");
     exit(-1);
   }
   else{
     printf("Destructed");      
   }  
#endif



        
  // anonymos test         
MESSAGE * file2 = m_connexion( NULL, O_CREAT|O_RDWR, 
                    5, 12, 0666);
if(!file2){
    printf("not created file");
}                    

#if 1
  int t3[] = {1, 2, 45}; /*valeurs à envoyer*/
  struct mon_message *m3 = malloc( sizeof( struct mon_message ) + sizeof( t3 ) );


   if( m3 == NULL ){
     /* traiter erreur de malloc */ 
     printf("Alloc error");
     return -1;  
    }
    m3->type = (long) getpid(); /* comme type de message, on choisit l’identité * de l’expéditeur */
    memmove( m3 -> mtext, t3, sizeof( t3 )) ; /* copier les deux int à envoyer */

    /* send in non-blocking mode */
   int i3 = m_envoi( file2, m3, sizeof( t3 ), O_NONBLOCK) ;
   if( i3 == 0 ){ /* message envoyé */ 
       printf("mesage is sent"); 
   }
   else if( i3 == -1 && errno == EAGAIN ){
      /* file pleine, essayer plus tard */
      printf(" file is full, emty it");
   }
   else{ /* erreur de la fonction */ 
     printf("error");
   }
   
   
   
  free(m3);  
  printf("State of Int: Len(%zu), Cap(%zu), CurrentSize (%zu) \n",
          m_message_len(file2),m_capacite(file2), m_nb(file2));
          
  printList(file2);   
  
#endif

#if 0
  int t4[] ={1,3,2,4,5};
  struct mon_message *m4 = (struct mon_message *) malloc( sizeof( struct mon_message ) + sizeof(t4));
  m4->type = (long) getpid(); /* comme type de message, on choisit l’identité * de l’expéditeur */
  memmove( m4 -> mtext, t4, sizeof( t4 )) ;
  /* send in non-blocking mode */
 i = m_envoi( file2, m4, 12, O_NONBLOCK) ;
   if( i == 0 ){ /* message envoyé */ 
       printf("mesage is sent"); 
   }
   else if( i == -1 && errno == EAGAIN ){
      /* file pleine, essayer plus tard */
      printf(" file is full, emty it");
   }
   else{ /* erreur de la fonction */ 
     printf("error");
   }
   
  free(m4);  

 
#endif 

  printf("State of Int: Len(%zu), Cap(%zu), CurrentSize (%zu) \n",
          m_message_len(file2),m_capacite(file2), m_nb(file2));
          
  printList(file2);   
  

  struct mon_message *mes03 = malloc( sizeof( struct mon_message ) + sizeof(t3));

  ssize_t rec03 = m_reception(file2, mes03, sizeof(t3), 0, O_NONBLOCK);

  if(rec03==0){
    printf("\nfirst: ");  
    printMessage2(mes03); 
  }
  free(mes03);
  
  printList(file2);   

#if 0
  struct mon_message *mes04 = malloc( sizeof( struct mon_message ) + sizeof(t3) );
  

  ssize_t rec04 = m_reception(file2, mes04, sizeof(t3), 0, O_NONBLOCK);

   
  if(rec04==0){
    printf("\nsecond: ");  
    printMessage2(mes04); 
  }
  else{
          printf("can't get second");  

    }
  free(mes04);
  
  printList(file2);   
 


  dic = m_deconnexion(file2);
  if (dic!=0){ 
     printf("Not disconnected");
     exit(-1);
  }
  else{
     printf("Disconnected");      
  }  
 
#endif    



  return 0;
}
