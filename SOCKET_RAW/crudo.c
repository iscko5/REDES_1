#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>

void obtenerDatos()
{
    struct ifreq nic;
    unsigned char nombre[20];
    printf("\nInserta el nombre de la interfaz");
    gets(nombre);
    strcpy(nic.ifr_name, nombre);
    if (ioctl(ds, SIOCGIFINDEX, &nic) == -1)
        ;
    {
        perror("\nError al obtener el indice");
    }
    else
    {
        printf("\nEl indice es: %s", nic.ifr_name);
    }
}

int main()
{
    // Here goes the code
    int packet_socket;
    // check manual man 7 packet
    packet_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (packet_socket == -1)
    {
        perror("\nError al abrir el socket");
        exit(0);
    }
    else
    {
        perror("\nExito al abrir el socket");
    }

    close(packet_socket);
    return 0;
}