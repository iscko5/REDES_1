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
    int serverSocket, tram, lbind;
    char buffer[1024];
    struct sockaddr_in servidor;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;

    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);

    if (serverSocket == -1)
    {
        perror("\nError al abrir el socket");
        exit(0);
    }
    else
    {
        perror("\nExito al abrir el socket.");
        servidor.sin_family = AF_INET;
        servidor.sin_port = htons(8080);
        servidor.sin_addr.s_addr = inet_addr("127.0.0.1");
        memset(servidor.sin_zero, '\0', sizeof servidor.sin_zero);

        lbind = bind(serverSocket, (struct sockaddr *)&servidor, sizeof(servidor));

        if (lbind == -1)
        {
            perror("\nError en bind");
            exit(0);
        }
        else
        {
            addr_size = sizeof serverStorage;

            printf("\nBienvenido al chat de la primera Practica");
            while (1)
            {
                tram = recvfrom(serverSocket, buffer, 1024, 0, (struct sockaddr *)&serverStorage, &addr_size);

                printf("\nCliente: %s", buffer);

                printf("\nManda mensaje al cliente: ");
                fgets(buffer, 1024, stdin);
                // printf("You typed: %s\n", buffer);

                tram = strlen(buffer) + 1;

                sendto(serverSocket, buffer, tram, 0, (struct sockaddr *)&serverStorage, addr_size);
            }
        }
    }
    close(serverSocket);
    return 0;
}