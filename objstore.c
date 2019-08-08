#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/uio.h>
#include "conn.h"
#include <pthread.h>
#include <ctype.h>
#include<sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>


void cleanup() {
    unlink(SOCKNAME);
}

void mystrok_r(char *buffer, char **data){
	char *token,*saveptr=NULL;
	char a[BUFSIZE];

	memset(a,'\0',BUFSIZE);
	memcpy(a,buffer,BUFSIZE);
	token=strtok_r(a,"\n",&saveptr);
	strcpy(*data,token);
	printf("contenuto di data in mystrok_r e'%s\n", *data);
}

int char_to_int(char* store_len){
	char *token, *saveptr=NULL;
	char buf[BUFSIZE];
	memset(buf,'\0',BUFSIZE);
	memcpy(buf,store_len,strlen(store_len));
	token=strtok_r(buf," ",&saveptr);
	if(token){
		token=strtok_r(NULL," ",&saveptr);
	}
	if(token){
		token=strtok_r(NULL," ",&saveptr);
	}
	printf("a%sa\n", token);
	long len= strtol(token, NULL, 10);

return len;
}

//creazione directory con il nome del client
int crea_directory(char *buffer,char **dir_name){  
	printf("SONO NELLA FUNZIONE CHE CREA LA DIRECTORY\n");
	int n=strlen(buffer);	
	char *a =  malloc((n+1)*sizeof(char));	//buffer ha dim BUFSIZE, copio nell'array a il contenuto significativo di buffer
	strncpy(a,buffer,n+1);
	char *token,*saveptr=NULL;
	char last_token[BUFSIZE];
	memset(last_token,'\0',BUFSIZE);

	token=strtok_r(a, " ", &saveptr);
	
	if(token){
		token=strtok_r(NULL," ",&saveptr);
	}
	
	strcpy(*dir_name,DIR_NAME); //copia il path in dir_name 
	strcat(*dir_name,token); //copia il nome della directory/client in dir_name
printf("nome cartella %s\n",*dir_name);
printf("ultimo token %s\n",token);
int res;
 	res=mkdir(*dir_name, 0777);
	    if(!res){ 
	    	printf("cartella creata\n" );
	    return 0;
		}
		if(res==-1){
				if(errno==EEXIST){
					printf("cartella esistente\n");
				}
				else{
					return -1;
				}
		}
return -1; 
	}

int store(char *buffer,char *dir_name, char* store_len,long connfd,int count){ //STORE + name + len \n + BLOCK 
	int dim_store_len=strlen(store_len)+2; // size di store ..len +\n + " " 
	int len=char_to_int(store_len); //ottengo la lunghezza len in int 
	char *token,*saveptr=NULL;
	char name[BUFSIZE];
	memset(name,'\0',BUFSIZE);
	char *a=buffer;
	
	token=strtok_r(a," ", &saveptr); // token = "STORE"
	
	if(token){ // path del file ./data/client/nome_file
		token=strtok_r(NULL, " ", &saveptr);
		strcpy(name,dir_name); //copio in name il percorso data/nome_client
		strcat(name,"/");
		strcat(name,token);
		printf("il primo token e' il nomessssssssssss %s del DATA \n",name);
	}
	
int fd;	
long res;
int n;

//creo il file nella directory del client 
printf("sto per aprire il file in lettura\n");
printf("il nome del file e' %s\n",name );
	SYSCALL(fd,open(name,O_CREAT | O_RDWR | O_TRUNC,0777),"chiamata open ");
printf("fd server open %d\n",fd);
printf("dimensione data %d\n",len );
printf("dim_store_len %d\n",dim_store_len );

	printf("COUNT %d\n",count );
	int b= count - dim_store_len;
	printf("b= %d\n", b);
	
	printf("la cosa che faccio sembra giusta:%s\n", &buffer[dim_store_len]);
	if(b>0){
		write(fd,&buffer[dim_store_len],b);
		len-=b; 
	}
		printf("len:%d\n",len );
	if(b<0){
		read(connfd,buffer,1); //leggo lo spazio vuoto
	}

	while(len>0){
		memset(buffer,'\0',BUFSIZE);
		n=read(connfd,buffer,BUFSIZE);
		printf("bytes letti %d\n",n );
			if(n>0){
				write(fd,buffer,n);
			}
		len-=n;
		printf("len %d\n", len);
	}

	close(fd);
	
return 0;
}

void retrieve(char* buffer,char *dir_name, char** block){
	char *name= malloc(BUFSIZE*sizeof(char)); 
	char *token,*saveptr=NULL,*last_token;
	last_token=malloc(BUFSIZE*sizeof(char));

	memset(name,'\0',BUFSIZE);

	token=strtok_r(buffer," ", &saveptr); // token = "RETRIEVE"
	while(token){
		strcpy(last_token,token);
		token=strtok_r(NULL," ", &saveptr); 
	}
	strcpy(name,dir_name); //copia il path in dir_name 
	strcat(name,"/");
	strcat(name,last_token);
	printf("file da trovareeeee %s\n", name);

	int fd;
	SYSCALL(fd,open(name,O_RDONLY,0777),"chiamata open "); //gestione errore lettura 
	int res_read;
	SYSCALL(res_read,read(fd,*block,BUFSIZE),"messaggio lettura: "); //VA BENE BUFSIZE?????
	//printf("IL CONTENUTO DEL BLOCCO E' %s\n", *block); 
}


// registazione client 
void *create_thread(void *arg){  

	assert (arg); // da rivedere 
	long connfd= (long) arg;
	int n; // numero di byte letti
	int res=0; //se res=0 creazione o accesso avvenuto con successo 
	char *buffer=malloc(BUFSIZE*sizeof(char));
	char *s=malloc(BUFSIZE*sizeof(char));
	char *token,*saveptr=NULL;
	int res_store;
	char *dir_name; //NOME CLIENT 
	dir_name= malloc(BUFSIZE*sizeof(char));
	char buf[BUFSIZE];
	int count;
	do{
		memset(buffer,'\0',BUFSIZE);
		memset(buf,'\0',BUFSIZE);
		memset(s,'\0',BUFSIZE);

		printf("il server sta per leggere\n");
		sleep(2);

		SYSCALL(count, readn_token(connfd, buffer), "read"); //leggo 10 bytes per discriminare l'header 
		//if(n<=0) break;
		printf("il risultato della read %d\n",n );


		printf("IL SERVER HA LETTO:%s\n", buffer );
		

		char request[11]; 
		memset(request,'\0',11);
		strncpy(request,buffer,10); //va bene ?

//printf("il server stampa il buffer letto %s\n",request );
		
		token= strtok_r(request," ",&saveptr);

printf("il server stampa il token %s\n",token );

	//REGISTER
	if(!strcmp(token,"REGISTER")){
		n=1;
		res=crea_directory(buffer,&dir_name); //dir_name -> ./data/client
		printf("STAMPA CODESTO PERCORSO %s\n", dir_name );
printf("END FUNCTION DIRECTORY\n");

printf("ORA VOGLIO SCRIVERE AL CLIENT \n");
		if(n>=0 || res==0){ //controllare se la guardia va bene 
			strcpy(s,"OK \n");

printf("%s\n",s );
		} 
		else{
			printf("creazionefallita \n");
			strcpy(s,"KO registrazione fallita \n"); //trova messaggio significativo 
		}
	}

	//STORE
	char *store_len=malloc(BUFSIZE*sizeof(char)); //stringa che conterra' STORE + nome + len
	memset(store_len,'\0',BUFSIZE);
	
	if(!strcmp(token,"STORE")){
		
		printf("STORE success\n" );
		mystrok_r(buffer,&store_len); //ottengo store + nome + len + " "
printf("STAMPA store_len e' %s\n",store_len);
printf("STRLEN store_len %ld\n",strlen(store_len) );


		res_store=store(buffer,dir_name,store_len,connfd,count);
		
		if(!res_store){
			strcpy(s,"OK \n");
printf("%s\n",s );
		}
		else{
			printf("memorizzazione fallita \n");
			strcpy(s,"KO memorizzazione fallita \n"); //trova messaggio significativo 
		}
	}

	//RETRIEVE
/*	if(!strcmp(token,"RETRIEVE")){
		char *buf_lenght= malloc(BUFSIZE*sizeof(char));
		char* block=malloc(BUFSIZE*sizeof(char)); 
		char len[BUFSIZE];
		memset(block,'\0',BUFSIZE);
		memset(len,'\0',BUFSIZE);

		printf("RETRIEVE success\n");
		retrieve(buffer,dir_name,&block);

		if(block!=NULL){
			//printf("l contenuto restituito dalla funzione retrieve e':%s\n",block );
			size_t len=strlen(block); // non metto +1 perche' nel contenuto di block c'e' gia' '\0'
			sprintf(buf_lenght,"%ld",len);
				strcpy(s,"DATA ");
				strcat(s,buf_lenght);
				strcat(s,"\n");
			//printf("%s\n",s );
				strcat(s,block);
						printf("il pacchetto da inviare al client e' %s\n", s);

		
		}
		else{
			printf("retrieve fallita \n");
			strcpy(s,"KO recupero dati fallito \n"); //trova messaggio significativo 
		}
	}
*/

/******************* SCRIVO AL CLIENT *******************/

printf("scrivo al client il risultato prodotto dal server\n");
		//SYSCALL(n, writen(conn_fd, s, strlen(s)+1), "write");
		int res_write;
		SYSCALL(res_write,write(connfd,s,strlen(s)+1), "error write");
printf("dopo la scrittura del server %s\n", s);
		//printf("il socket del server e' %ld\n", conn_fd);

//free(buffer);

    	} while(1);
	close(connfd);	    

// 	return NULL;
} //fine thread 

// creazione thread che gestira' le richieste del client 
void new_thread(long conn_fd){
	pthread_attr_t thattr;
    pthread_t thid;

    if (pthread_attr_init(&thattr) != 0) {
	fprintf(stderr, "pthread_attr_init FALLITA\n");
	close(conn_fd);
	return;
    }
    // settiamo il thread in modalità detached
    if (pthread_attr_setdetachstate(&thattr,PTHREAD_CREATE_DETACHED) != 0) {
	fprintf(stderr, "pthread_attr_setdetachstate FALLITA\n");
	pthread_attr_destroy(&thattr);
	close(conn_fd);
	return;
    }
    if (pthread_create(&thid, &thattr, create_thread, (void*)conn_fd) != 0) {  
	fprintf(stderr, "pthread_create FALLITA");
	pthread_attr_destroy(&thattr);
	close(conn_fd);
	return;
    }
	
}

int main(int argc, char *argv[]){

long conn_fd; 

/*****************************CREAZIONE SOCKET*****************************/
	cleanup();    
    atexit(cleanup);

	int obj_fd;
	//creo il socket 
	SYSCALL(obj_fd,socket(AF_UNIX,SOCK_STREAM,0),"socket");

	// setto indirizzo 
	struct sockaddr_un sa;
    memset(&sa, '0', sizeof(sa));
    sa.sun_family = AF_UNIX;    
    strncpy(sa.sun_path, SOCKNAME, strlen(SOCKNAME)+1); // dim sunpath[] dovrebbe essere UNIX_PATH_MAX 108

 	//assegno indirizzo al socket 
    int foo;
 	SYSCALL(foo, bind(obj_fd, (struct sockaddr*)&sa,sizeof(sa)), "bind");

 	SYSCALL(foo, listen(obj_fd, MAXBACKLOG), "listen"); 
	

 	while(1){

 		// mi metto in attesa sulla accept, finche' non c'e' una richiesta di un client
		SYSCALL(conn_fd, accept(obj_fd, (struct sockaddr*)NULL ,NULL), "accept"); 
		// restituisce il conn_fd del socket, usato per la comunicazione client-server
	printf("connessione server avvenuta con successo\n");
		// creo il thread, che gestira' il client 
		new_thread(conn_fd); 
		
 		
 	}
 
return 0;
}