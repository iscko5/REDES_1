#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>

int main()
{
int udp_socket,lbind, tam, lrecv;
unsigned char paqRec[512];
struct sockaddr_in servidor, cliente;
udp_socket=socket(AF_INET, SOCK_DGRAM, 0);
if(udp_socket==-1)
	{
		perror("\nError al abrir el socket");
		exit(0);
	}
else
	{
		perror("\nExito al abrir el socket");
		servidor.sin_family=AF_INET;
		servidor.sin_port=htons(8080);
		servidor.sin_addr.s_addr=INADDR_ANY;
		lbind=bind(udp_socket,(struct sockaddr *)&servidor,sizeof(servidor));
	if(lbind==-1)
	{
	perror("\nError en bind");
		exit(0);
	}
	else{
	perror("\nExito en bind");
	lrecv=sizeof(cliente);
	tam=recvfrom(udp_socket,paqRec,512,0,(struct sockaddr *)&cliente,&lrecv);
	if(tam==-1){
		perror("\nError al recibir");
		exit(0);
	}
	else{
		printf("\nEl mensaje es: %s", paqRec);
	}
	}
	}
close(udp_socket);
return 0;
}
