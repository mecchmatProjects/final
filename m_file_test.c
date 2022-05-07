#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

#include "m_file.h"

//И будут ли как-то оцениваться собственные тесты

int main(int argc, char* argv[]){

   // single-thread:            
  //1. given test         

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
  else{
          printf("can't get first message");  
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
          printf("can't get second message");  
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
printf("\n\n\n\n\n\n Char messages test\n");

  //basic test
  char* buf[] ={"mes1","messs2","me3", "mes4","m5"};
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
  
  printf("\n\n recieve 1st message:\n");

  printf("State of MESSAGE: Len(%zu), Cap(%zu), CurrentSize (%zu)\n",
          m_message_len(MES_BASE),m_capacite(MES_BASE), m_nb(MES_BASE));
  
  printList(MES_BASE);   
  
  free(mes2);
  
  printf("\n\n Receive 5 messages:\n");
  
   for(int i=0;i<5;i++){
     struct mon_message *mes3 = malloc( sizeof( struct mon_message ) + sizeof( strlen(buf[i])+1 ) );

     printf("expected message of size %d", strlen(buf[i])+1);
     ssize_t rec = m_reception(MES_BASE, mes3,  strlen(buf[i])+1, 0, O_NONBLOCK);
  
     if(rec<0){
        printf("\nNo msg received %zd\n",rec);
     }
     else{
        printf("\nMsg received %zd, ",rec);
        printf("Msg=%s\n", mes3->mtext);  
     }
  
     printf("State of MESSAGE: Len(%zu), Cap(%zu), CurrentSize (%zu)\n",
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

////////////////////////////      
// anonymous test        

printf("\n\n\n\n\n\n Anonymous test\n");
#if 1
 
MESSAGE * file2 = m_connexion( NULL, O_CREAT|O_RDWR, 
                    5, 12, 0666);
if(!file2){
    printf("not created file");
}                    


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
    
  //free(m3);  
  printf("State of Int: Len(%zu), Cap(%zu), CurrentSize (%zu) \n",
          m_message_len(file2),m_capacite(file2), m_nb(file2));
          
  printList(file2);   
  
#endif

#if 1
  int t4[] ={1,3,2,4,5};
  // Memory leak????
  //struct mon_message *m4 = (struct mon_message *) malloc( sizeof( struct mon_message ) + sizeof(t4));
  //m4->type = (long) getpid(); /* comme type de message, on choisit l’identité * de l’expéditeur */
  //memmove( m4 -> mtext, t4, sizeof( t4 )) ;
  /* send in non-blocking mode */
 i = m_envoi( file2, m3, 12, O_NONBLOCK) ;
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
   
  free(m3);  

 
#endif 
#if 1
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

// parallel anonymous test
printf("\n\n\n\n\n\n Parallel Anonymous test\n");

 
 MESSAGE * file_anon2 = m_connexion( NULL, O_CREAT|O_EXCL|O_RDWR, 
                    3, 12, 0666);
     

  pid_t pd = fork();

  if(pd==-1){
   
        fprintf(stderr, "fork() failed\n");
        exit(EXIT_FAILURE);    
  }
  else if(pd==0){         /*Child: wreite read*/     
     
      printf("In child");
      int tchild[3] = {1, 2, 45}; /*valeurs à envoyer*/
      struct mon_message *mes_child = malloc( sizeof( struct mon_message ) + sizeof( tchild) );
      
      mes_child->type = (long) getpid();
      memmove( mes_child-> mtext, tchild, sizeof( tchild )) ;               
      
      int i_an = m_envoi( file_anon2, mes_child, sizeof( tchild ), 0) ;
      if( i_an == 0 ){ /* message envoyé */ 
         printf("mesage is sent:\n"); 
         printList(file_anon2);   

      }
      else if( i_an == -1 && errno == EAGAIN ){
        
         printf(" file is full, emty it");
      }
      else{ /* erreur de la fonction */ 
       printf("error");
     }
           
       
      free(mes_child); 
      printf("\nOut file:\n");
      printList(file_anon2);   

      
      dic = m_deconnexion(file_anon2);
      if (dic!=0){ 
           printf("Not disconnected");
           exit(-1);
      }
      else{
           printf("Disconnected");      
       }  
      
  }
  else{         /*Parent: wait for child to terminate*/      
       if (wait(NULL) == -1) {    
            fprintf(stderr, "wait() failed\n");    
            exit(EXIT_FAILURE);      
        }     

      printf("In parent");         
      int tparent[3] = {0,}; /*valeurs à envoyer*/
      struct mon_message *mes_parent = malloc( sizeof( struct mon_message ) + sizeof( tparent ));
          
      /*MESSAGE * file_anon2_1 = m_connexion( NULL, O_EXCL|O_RDONLY, 
                    3, 12, 0666);*/
      
      
      int i_an21 = m_reception( file_anon2, mes_parent, sizeof(tparent), 0, 0);
      if( i_an21 == 0 ){ /* message envoyé */ 
         printf("mesage is get:");
         printMessage2(mes_parent); 
      }
      else if( i_an21 == -1 && errno == EAGAIN ){
        
         printf(" file is full, emty it");
      }
      else{ /* erreur de la fonction */ 
       printf("error");
     }
           
      printList(file_anon2);  
      free(mes_parent); 
      
      dic = m_deconnexion(file_anon2);
      if (dic!=0){ 
           printf("Not disconnected");
           exit(-1);
      }       
   }   



// parallel non-anonymous test
printf("\n\n\n\n\n\n Parallel Forked test\n");
 
MESSAGE * file_forked = m_connexion( "forked.dat", O_CREAT|O_EXCL|O_RDWR, 
                    3, 12, 0666);
     

  pd = fork();

  if(pd==-1){
   
        fprintf(stderr, "fork() failed\n");
        exit(EXIT_FAILURE);    
  }
  else if(pd==0){         /*Child: wreite read*/     
     
      printf("In child");
      int tchild[3] = {5, 6, 7}; /*valeurs à envoyer*/
      struct mon_message *mes_child = malloc( sizeof( struct mon_message ) + sizeof( tchild) );
      
      mes_child->type = (long) getpid();
      memmove( mes_child-> mtext, tchild, sizeof( tchild )) ;               
      
      int i_an = m_envoi( file_forked, mes_child, sizeof( tchild ), 0) ;
      if( i_an == 0 ){ /* message envoyé */ 
         printf("mesage is sent:\n"); 
         printList(file_forked);   

      }
      else if( i_an == -1 && errno == EAGAIN ){
        
         printf(" file is full, emty it");
      }
      else{ /* erreur de la fonction */ 
       printf("error");
     }
           
       
      free(mes_child); 
      printf("\nOut file:\n");
      printList(file_forked);   

      
      dic = m_deconnexion(file_forked);
      if (dic!=0){ 
           printf("Not disconnected");
           exit(-1);
      }
      else{
           printf("Disconnected");      
       }  
      
  }
  else{         /*Parent: wait for child to terminate*/      
       if (wait(NULL) == -1) {    
            fprintf(stderr, "wait() failed\n");    
            exit(EXIT_FAILURE);      
        }     

      printf("In parent");         
      int tparent[3] = {0,}; /*valeurs à envoyer*/
      struct mon_message *mes_parent = malloc( sizeof( struct mon_message ) + sizeof( tparent ));
          
      /*MESSAGE * file_anon2_1 = m_connexion( NULL, O_EXCL|O_RDONLY, 
                    3, 12, 0666);*/
      
      
      int i_an21 = m_reception( file_forked, mes_parent, sizeof(tparent), 0, 0);
      if( i_an21 == 0 ){ /* message envoyé */ 
         printf("mesage is get:");
         printMessage2(mes_parent); 
      }
      else if( i_an21 == -1 && errno == EAGAIN ){
        
         printf(" file is full, emty it");
      }
      else{ /* erreur de la fonction */ 
       printf("error");
     }
           
      printList(file_forked);  
      free(mes_parent); 
      
      dic = m_deconnexion(file_forked);
      if (dic!=0){ 
           printf("Not disconnected");
           exit(-1);
      }

     int destr1 = m_destruction("forked.dat");
     if (destr1!=0){ 
     printf("Not destructed");
     exit(-1);
   }
   else{
     printf("Destructed");      
   }         
      
   }   

// parallel read by forked test
printf("\n\n\n\n\n\n Parallel Read Forked test\n");
 
  pd = fork();

  if(pd==-1){
   
        fprintf(stderr, "fork() failed\n");
        exit(EXIT_FAILURE);    
  }
  else if(pd==0){         /*Child: write read*/     
     
      printf("In child");
      int tchild[3] = {8, 9, 5}; /*valeurs à envoyer*/
      struct mon_message *mes_child = malloc( sizeof( struct mon_message ) + sizeof( tchild) );
      
      
      MESSAGE * file_forked2_1 = m_connexion( "forked2.dat", O_CREAT|O_EXCL|O_RDWR, 
                    3, 12, 0666);
     
      mes_child->type = (long) getpid();
      memmove( mes_child-> mtext, tchild, sizeof( tchild )) ;               
      
      int i_an = m_envoi( file_forked2_1, mes_child, sizeof( tchild ), 0) ;
      if( i_an == 0 ){ /* message envoyé */ 
         printf("mesage is sent:\n"); 
         printList(file_forked2_1);   

      }
      else if( i_an == -1 && errno == EAGAIN ){
        
         printf(" file is full, emty it");
      }
      else{ /* erreur de la fonction */ 
       printf("error");
     }
           
       
      free(mes_child); 
      printf("\nOut file:\n");
      printList(file_forked2_1);   

      
      dic = m_deconnexion(file_forked2_1);
      if (dic!=0){ 
           printf("Not disconnected");
           exit(-1);
      }
      else{
           printf("Disconnected");      
       }  
      
  }
  else{         /*Parent: wait for child to terminate*/      
       if (wait(NULL) == -1) {    
            fprintf(stderr, "wait() failed\n");    
            exit(EXIT_FAILURE);      
        }     

      printf("In parent");         
      int tparent[3] = {0,}; /*valeurs à envoyer*/
      struct mon_message *mes_parent = malloc( sizeof( struct mon_message ) + sizeof( tparent ));
          
      MESSAGE * file_forked2_2 = m_connexion( "forked2.dat", O_EXCL|O_RDWR, 
                    3, 12, 0666);
      
      
      int i_an21 = m_reception( file_forked2_2, mes_parent, sizeof(tparent), 0, 0);
      if( i_an21 == 0 ){ /* message envoyé */ 
         printf("mesage is get:");
         printMessage2(mes_parent); 
      }
      else if( i_an21 == -1 && errno == EAGAIN ){
        
         printf(" file is full, emty it");
      }
      else{ /* erreur de la fonction */ 
       printf("error");
     }
           
      printList(file_forked2_2);  
      free(mes_parent); 
      
      dic = m_deconnexion(file_forked2_2);
      if (dic!=0){ 
           printf("Not disconnected");
           exit(-1);
      }

     int destr1 = m_destruction("forked2.dat");
     if (destr1!=0){ 
     printf("Not destructed");
     exit(-1);
   }
   else{
     printf("Destructed");      
   }         
      
   }   






  return 0;
}
