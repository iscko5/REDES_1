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
	int udp_socket_servidor, lbind, tam, lrecv;
	unsigned char buffer[1024];
	struct sockaddr_in servidor, cliente;
	udp_socket_servidor = socket(AF_INET, SOCK_DGRAM, 0);
	if (udp_socket_servidor == -1)
	{
		perror("\nError al abrir el socket");
		exit(0);
	}
	else
	{
		perror("\nExito al abrir el socket");
		servidor.sin_family = AF_INET;
		servidor.sin_port = htons(8080);
		servidor.sin_addr.s_addr = inet_addr("192.168.100.27");
		lbind = bind(udp_socket_servidor, (struct sockaddr *)&servidor, sizeof(servidor));
		if (lbind == -1)
		{
			perror("\nError en bind");
			exit(0);
		}
		else
		{
			perror("\nExito en bind");
			lrecv = sizeof(cliente);
			while (1)
			{
				tam = recvfrom(udp_socket_servidor, buffer, 1024, 0, (struct sockaddr *)&cliente, &lrecv);
				if (tam == -1)
				{
					perror("\nError al recibir");
					exit(0);
				}
				else
				{
					printf("\nEl mensaje es: %s", buffer);
				}
				printf("\nEscribe otra sentencia para el cliente: ");
				fgets(buffer, 1024, stdin);

				tam = strlen(buffer) + 1;

				sendto(udp_socket_servidor, buffer, bind, 0, (struct sockaddr *)&cliente, &lrecv);
			}
		}
	}
	close(udp_socket_servidor);
	return 0;
}
