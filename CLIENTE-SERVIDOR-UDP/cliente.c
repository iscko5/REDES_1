#include<sys/socket.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>

int main()
{
int udp_socket,lbind, tam;
unsigned char msj[100]="Hola red, buenas";
struct sockaddr_in local, remota;
udp_socket=socket(AF_INET, SOCK_DGRAM, 0);
if(udp_socket==-1)
	{
		perror("\nError al abrir el socket");
		exit(0);
	}
else
	{
		perror("\nExito al abrir el socket");
		local.sin_family=AF_INET;
		local.sin_port=htons(0);
		local.sin_addr.s_addr=INADDR_ANY;
		lbind=bind(udp_socket,(struct sockaddr *)&local,sizeof(local));
	if(lbind==-1)
	{
	perror("\nError en bind");
		exit(0);
	}
	else{
	perror("\nExito en bind");
	remota.sin_family=AF_INET;
	remota.sin_port=htons(8080);
	remota.sin_addr.s_addr=inet_addr("192.168.1.121");
	tam=sendto(udp_socket,msj,20,0,(struct sockaddr *)&remota,sizeof(remota));
	if(tam==-1){
		perror("\nError al enviar");
		exit(0);
	}
	else{
		perror("\nExito al enviar");
	}
	}
	}
close(udp_socket);
return 0;
}
