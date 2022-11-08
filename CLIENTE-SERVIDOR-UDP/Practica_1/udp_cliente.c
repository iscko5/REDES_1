#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

int main()
{
    int clientSocket, tram, lbind;
    char buffer[1024];
    struct sockaddr_in cliente;
    socklen_t addr_size;

    clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == -1)
    {
        perror("\nError al abrir el socket");
        exit(0);
    }
    else
    {
        cliente.sin_family = AF_INET;
        cliente.sin_port = htons(8080);
        cliente.sin_addr.s_addr = inet_addr("127.0.0.1");
        memset(cliente.sin_zero, '\0', sizeof cliente.sin_zero);

        // lbind = bind(clientSocket, (struct sockaddr *)&cliente, sizeof(cliente));

        addr_size = sizeof cliente;

        printf("\nBienvenido al chat de la primera Practica");
        printf("\nEscribe exit para dejar de enviar mensajes\n");
        while (1)
        {

            printf("\nManda mensaje al servidor: ");
            fgets(buffer, 1024, stdin);

            // if (buffer == "exit")
            // {
            //     break;
            // }

            tram = strlen(buffer) + 1;

            sendto(clientSocket, buffer, tram, 0, (struct sockaddr *)&cliente, addr_size);

            tram = recvfrom(clientSocket, buffer, 1024, 0, NULL, NULL);

            printf("\nServidor: %s", buffer);
        }
    }
    close(clientSocket);
    return 0;
}