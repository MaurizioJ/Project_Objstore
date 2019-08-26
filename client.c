#include <unistd.h>
#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include "conn.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<math.h>


#include"lib_client.h"

int main(int argc, char *argv[]) {
	if(argc!=2){
		fprintf(stderr, "usa: %s nome_client\n", argv[0]);
	return -1;
	}
	if(strlen(argv[1])>BUFSIZE){ //controllo nome client che non sia piu' lungo di 256 caratteri
		fprintf(stderr, "nome_client troppo lungo \n");
	return -1;
	}

	char* client=malloc(sizeof(char)*(strlen(argv[1]) +1) );
	strcpy(client,argv[1]);
	int res_store; 
	int res,n,del;
	res=os_connect(client);
printf("RISULTATO OS_CONNECT %d\n",res);
	int pos_fd; 

	
	if(res){ // connessione avvenuta con successo
	
		if((pos_fd= open("./bibbo.txt",O_RDWR,0777))==-1){ // apro il file in lettura 
			perror("bibbo.txt in apertura");
			exit(EXIT_FAILURE);
		}

//printf("fd bibbo %d\n",pos_fd); 
		//preparo il data da mandare al server
		char  name_data[BUFSIZE];
		memset(name_data,'\0',BUFSIZE);
		strcpy(name_data,"bibbo.txt"); 
		if(strlen(name_data)>BUFSIZE){ //controllo lunghezza nome del file 
			fprintf(stderr, "%s: nome_file troppo lungo \n", name_data);
		return -1;
		}
printf("STAMPA NOME FILE %s\n",name_data ); // stampa nome file
		size_t len;  
		struct stat statbuf;

		stat(name_data, &statbuf);
		len=statbuf.st_size; // dimensione name_data
printf("dim file %ld\n", len );
		char *block=malloc(len*sizeof(char));
		memset(block,'\0',len);
		SYSCALL(n,read(pos_fd,block,len),"read data"); //leggo il file aperto 

		
//printf("%ld\n", len); // stampa lunghezza file

printf("ENTRO NELLA OS_STORE\n");
		res_store=os_store(name_data,block,len);
printf("USCITA OS_STORE");
printf("RISULTATO STORE %d\n", res_store);
	//res_store=os_store("pippo","ciao scemo",10);*/
//	int p[5]={1,2,3,4,5};
//	res_store=os_store("deh",p,5*sizeof(int));
		//recupero oggetto precedentemente memorizzato 
		if(res_store){
			char *res_retrieve;
			res_retrieve=(char*) os_retrieve(name_data);
		//	printf("il risultato della funzione os_retrieve e' %s\n",res_retrieve);
		}
	
		else{
			printf("memorizzazione dati fallita\n");
			//recupero dati fallito
		}
	
			del=os_delete(name_data);
	
	}

	else{
		// gestione errore, connessione non avvenuta
	}

return 0;
}