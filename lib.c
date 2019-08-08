#include<string.h>
#include <unistd.h>
#include <stdio.h>
#include "conn.h"
#include <assert.h>
#include <stdlib.h>
#include "lib_client.h"

static int sock_fd;

/*int request(char *name,size_t len, char *buffer){
  int n;
	
  
  char *buf_s; //buffer nel quale memorizzo lunghezza len 
 	buf_s= malloc(BUFSIZE*sizeof(char));
  memset(buf_s,'\0',BUFSIZE);
  sprintf(buf_s,"%ld",len); //converto da int a char 
  n=strlen(buf_s)+1;
  //rialloco buf_s che contiene il numero di byte di block
  char *p=realloc(buf_s,n*sizeof(char));
  if(!p){
     perror("error realloc len: ");
  }
  else{ 
    buf_s=p;
  }
//printf("daaaaaaaaadadad%s\n",buf_s);

  int res=snprintf(buffer,BUFSIZE,"STORE %s %d \n ",name,len);
printf("snprintf %d\n",res);
int size=res+len;
  char *b=realloc(*buffer,sizeof(char)*size);

    if(!b){
      perror("error realloc buffer: ");
    }
    else{
      *buffer=b;
    }
  memcpy(&buffer,block,len);
printf("il buffer in request e' %s\n",*buffer );

return res;
}*/

void mystrok_r(char *response, char **data){
char *token,*saveptr=NULL;

  token=strtok_r(response,"\n",&saveptr);
  strcpy(*data,saveptr);

}
 	
int os_connect(char *name){
    int foo; 
    int res=-1; //risultato connession
    
	if(name==NULL){ //DA RIVEDERE
		exit(EXIT_FAILURE); // USA CHECKNULL 
	}
	
	struct sockaddr_un sa;
	
	//creo il socket 
	SYSCALL(sock_fd, socket(AF_UNIX, SOCK_STREAM, 0), "socket");
    memset(&sa, '0', sizeof(sa));

    sa.sun_family = AF_UNIX;    
    strncpy(sa.sun_path,SOCKNAME, strlen(SOCKNAME)+1);
	  SYSCALL(res, connect(sock_fd, (struct sockaddr*)&sa, sizeof(sa)), "connect");
    // connessione client-server avvenuta 
	
   	char buffer[BUFSIZE]; 
    /*memset(buffer,'\0',BUFSIZE); // METTO /0 IN TUTTE LE POS DEL BUFFER 

/************************* WRITE AL SERVER *************************/
    int rw;
    snprintf(buffer,BUFSIZE,"REGISTER %s \n",name);
  	SYSCALL(rw,write(sock_fd,buffer,strlen(buffer)+1),"write");

//printf("il socket del client e' %d\n", sock_fd);
//printf("%s\n", buffer);
//printf("dimensione buffer %ld\n",strlen(buffer)+1);

/************************* RISPOSTA DEL SERVER *************************/
	//leggo la risposta del server 
	char response[BUFSIZE];
  char *token;
  char **saveptr=NULL;
	memset(response,'\0',BUFSIZE);
  
    SYSCALL(rw,read(sock_fd, response, BUFSIZE),"read");
    //printf("responso\n");
    fprintf(stdout, "riposta ricevuta dal server: %s\n", response);
    token=strtok(response, " ");
    if(!strcmp(token,"OK")){

    	//printf("1\n");
 		res=1;
 	  }
   	
    else{
   	//	printf("registrazione fallita!\n");
   		res=0;
   	}
return res; 
}


int os_store(char *name, void *block, size_t len){
	
	char* buffer= malloc(BUFSIZE*sizeof(char)); 
  memset(buffer,'\0',BUFSIZE); // METTO \0 IN TUTTE LE POS DEL BUFFER 

	//int res_req=request(name,len,buffer); // STORE + name + len 

  int res_req=snprintf(buffer,BUFSIZE,"STORE %s %ld \n ",name,len);

printf("snprintf %d\n",res_req);
int size=res_req+len;
  char *b=realloc(buffer,sizeof(char*)*size);
   if(!b){
      perror("error realloc buffer ");
    }
    else{
      buffer=b;
    }
  memcpy(&buffer[res_req],block,len);
//printf("il buffer in request e' %s\n",buffer );
	//printf("IL BUFFER DELLA STORE PRIMA DELLA SCRITTURA NEL SERVER e': %s\n", buffer);
	//printf("la dim del buffer e' %ld\n",strlen(buffer) );

	write (sock_fd,buffer,size);

/******** LEGGO LA RISPOSTA DEL SERVER ********/
 char response[BUFSIZE];
  char *token;
  
  int res;

  memset(response,'\0',BUFSIZE);
    //SYSCALL(foo, readn(sock_fd, response, BUFSIZE), "read");  
    read(sock_fd, response, BUFSIZE);
    //printf("responso\n");
    fprintf(stdout, "%s\n", response);
    token=strtok(response, " ");
    if(!strcmp(token,"OK")){

      //printf("1\n");
    res=1;
    }
    
    else{
    //  printf("memorizzazione fallita!\n");
      res=0;
    }

return res; 
}


/*void *os_retrieve(char *name){

    char buffer[BUFSIZE]; 
    memset(buffer,'\0',BUFSIZE); // METTO /0 IN TUTTE LE POS DEL BUFFER 
    char reg[10]="RETRIEVE ";
    strcpy(buffer,reg); //copio la stringa RETRIEVE NEL BUFFER
    strcat(buffer,name); //concateno il nome del client con la stringa RETRIEVE
    // scrivo al server 
    write(sock_fd,buffer,strlen(buffer)+1);
    //printf("il socket del client e' %d\n", sock_fd);
    printf("RETRIEVE BUFFER %s\n", buffer);
  */
   /******** LEGGO LA RISPOSTA DEL SERVER ********/
  /*char response[BUFSIZE];
  char *token;
  char *last_token=malloc(BUFSIZE*sizeof(char));
  char *tok_len=malloc(BUFSIZE*sizeof(char));
  char *tok_block=malloc(BUFSIZE*sizeof(char));
  memset(last_token,'\0',BUFSIZE);
  memset(tok_len,'\0',BUFSIZE);
  memset(tok_block,'\0',BUFSIZE);
  memset(response,'\0',BUFSIZE);
  char *saveptr=NULL;
  int res;
  char *block;
  char *resp=malloc(BUFSIZE*sizeof(char));
    //SYSCALL(foo, readn(sock_fd, response, BUFSIZE), "read");  
    
    read(sock_fd, response, BUFSIZE); 
    strncpy(resp,response,BUFSIZE); // servira' per la mystrok_r MEMCPY 
    printf("risposta SERVER RETRIEVE %s\n", response);
    token=strtok_r(response," ",&saveptr);

    if(!strcmp(token,"DATA")){ // controllo se il primo token sia DATA 
    //  printf("%s\n",token );
      if(token){
          token=strtok_r(NULL," \n ",&saveptr);
          strcpy(tok_len,token);
        printf("token len %s\n", tok_len);
      }

     /* if(token){
          token=strtok_r(NULL,"\n",&saveptr);
          strcpy(tok_block,token);
        printf("token BLOCK %s\n",tok_block);
      }*/

 /*     size_t len;
      char *endptr;
      len=strtol(tok_len, &endptr, 10);
      printf("la lunghezza della stringa e' %ld\n",len );
      size_t left=len - strlen(tok_block); //quanti bytes mi restano da leggere
        
        if(left==0){ //nel buffer sono riuscito a scrivere tutto in un solo colpo 
            return tok_block;
        }  

        if(left>0){ 
          char *data=malloc(sizeof(char)*len);
          mystrok_r(resp,&data); // ottengo i bytes data
          printf("la mia strok produce %s\n",data );
          int n;
          block=malloc(len*sizeof(char));
          memset(block,'\0',len);
          strcat(block,tok_block); //in block ci copio i bytes che avevo gia' letto

            data+=strlen(tok_block)+1;
            while(left>0){
              n=read(sock_fd,data,left);
              left-=n;
              strcat(block,data);
              data+=n;
             }
          
        printf("il block stampato e' %s\n",block );
        return block;
      }
    }
    else{
      //stampa KO
    }

}
*/