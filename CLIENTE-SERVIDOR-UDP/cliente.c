#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main()
{
	int udp_socket_client, lbind, tam, lrecv;
	unsigned char msj[100];
	struct sockaddr_in local; // remota
	udp_socket_client = socket(AF_INET, SOCK_DGRAM, 0);
	if (udp_socket_client == -1)
	{
		perror("\nError al abrir el socket");
		exit(0);
	}
	else
	{
		perror("\nExito al abrir el socket");
		local.sin_family = AF_INET;
		local.sin_port = htons(8080);
		local.sin_addr.s_addr = inet_addr("192.168.1.121");
		lbind = bind(udp_socket_client, (struct sockaddr *)&local, sizeof(local));
		if (lbind == -1)
		{
			perror("\nError en bind");
			exit(0);
		}
		else
		{
			perror("\nExito en bind");
			lrecv = sizeof(local);
			while (1)
			{
				printf("\nEscribe una sentencia al servidor: ");
				fgets(buffer, 1024, stdin);

				tam = strlen(buffer, 1024, stdin);

				sendto(udp_socket_client, buffer, tam, 0, (struct sockaddr *)&local, &lrecv);

				tam = recvfrom(udp_socket_client, buffer, 1024, 0, NULL, NULL);

				printf("\nEl mensaje recibido: %s", buffer);
			}
			// remota.sin_family=AF_INET;
			// remota.sin_port=htons(8080);
			// remota.sin_addr.s_addr=inet_addr("192.168.1.121");
			// tam=sendto(udp_socket_client,msj,20,0,(struct sockaddr *)&remota,sizeof(remota));

			// if(tam==-1){
			// 	perror("\nError al enviar");
			// 	exit(0);
			// }
			// else{
			// 	perror("\nExito al enviar");
			// }
		}
	}
	close(udp_socket_client);
	return 0;
}
