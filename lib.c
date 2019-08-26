#include<string.h>
#include <unistd.h>
#include <stdio.h>
#include "conn.h"
#include <assert.h>
#include <stdlib.h>
#include<math.h>
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
	
	int len= 12 + strlen(name) + 1;
   	char* buffer= malloc(len*sizeof(char));
   	if(!buffer){
   		perror("error malloc buffer ");
   	}
    /*memset(buffer,'\0',BUFSIZE); // METTO /0 IN TUTTE LE POS DEL BUFFER 

/************************* WRITE AL SERVER *************************/
    int rw;
    snprintf(buffer,len,"REGISTER %s \n",name);
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
	double n= 12 + strlen(name) + 1 + floor(log10(len))+1;
	char* buffer= malloc(n*sizeof(char)); 
  	memset(buffer,'\0',n); // METTO \0 IN TUTTE LE POS DEL BUFFER 

	//int res_req=request(name,len,buffer); // STORE + name + len 

  int res_req=snprintf(buffer,n,"STORE %s %ld \n ",name,len);

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


void *os_retrieve(char *name){

	int rw,count=0;
	int n= strlen(name)+ 1 + 12;
	char *buffer=malloc(n*sizeof(char));
	memset(buffer,'\0',n); // METTO \0 IN TUTTE LE POS DEL BUFFER 
	int size=snprintf(buffer,n,"RETRIEVE %s \n",name);
	printf("buffer retrieve %s\n", buffer);
	SYSCALL(rw,write(sock_fd,buffer, size), "write");

   /******** LEGGO LA RISPOSTA DEL SERVER ********/
	char* res;
	char* response= malloc(BUFSIZE*sizeof(char));
	char *token,*saveptr=NULL;
	char* tmp=malloc(BUFSIZE*sizeof(char));
	char* last_token=malloc(30*sizeof(char)); 
	SYSCALL(count, readn_token(sock_fd, response), "read");
	printf("response:%s\n",response );
	memset(tmp,'\0',BUFSIZE);
	memcpy(tmp,response,count+1);
	token=strtok_r(tmp," ",&saveptr); //token=DATA
	printf("token %s\n",token );
	if(token){
		token=strtok_r(NULL," ", &saveptr); 
		strcpy(last_token,token); //last_token salvera' il contenuto di len 
	}
	printf("last_token %s\n",last_token );
	long len= strtol(last_token, NULL, 10);
	printf("aaaa%ld\n",len );
	int dim_data_len= 8+(floor(log10(len))+1);//7=DATA + 2 SPAZI + \N + " "
	printf("lallalal%d\n",dim_data_len);
	int b = count - dim_data_len; 
	printf("b sara' %d\n",b );
	char* tmp_response= realloc(response,len*sizeof(char));
		if(!tmp_response){
			res=NULL;
			perror("error invalid realloc");
		}
	printf("il client ha letto dal server %saaa\n", last_token);
	
 	if(b<0){
 		read(sock_fd,response,1); //leggo lo spazio vuoto dopo \n 
 	}
 	res=malloc(len*sizeof(char));
 	if(b>0){
		memcpy(res,&response[dim_data_len],b);
		len-=b; 
	}
	printf("res: %s\n", res);
	printf("dopo %c\n", response[dim_data_len]);

	int nb=0;
	while(len>0){
		memset(response,'\0',BUFSIZE);
		nb=read(sock_fd,response,BUFSIZE);
			if(nb>0){
				strncat(res,response,nb);
			}
	len-=nb;
	}
	//printf("buffer res %s\n", res);
return res;
//DEFINIRE TUTTI I CASI DI ERRORI E QUINDI SETTARE RES = NULL 
}

int os_delete(char* name){

	int rw;
	int n= strlen(name)+ 1 + 10;
	char *buffer=malloc(n*sizeof(char));
	memset(buffer,'\0',n); // METTO \0 IN TUTTE LE POS DEL BUFFER 
	int size=snprintf(buffer,n,"DELETE %s \n",name);
	printf("buffer %s\n", buffer);
	SYSCALL(rw,write(sock_fd,buffer, size), "write");

   /******** LEGGO LA RISPOSTA DEL SERVER ********/
	
	read(sock_fd,buffer,BUFSIZE);
	
	char *saveptr=NULL;
	char* token= strtok_r(buffer," ",&saveptr);
	
	if(!strcmp(buffer,"OK")){
		printf("cancellazione effettuata\n");
		return 1;
	}
printf("%s\n",buffer );
return 0;
}