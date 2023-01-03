#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> /* the L2 protocols */
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
unsigned char MACorigen[6];
unsigned char ip[4];
unsigned char mascara[4];
unsigned char MACbroad[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
unsigned char ethertype[2] = {0x0C, 0x0C};
unsigned char tramaEnv[1514];
int obtenerDatos(int ds)
{
    struct ifreq nic;
    unsigned char nombre[20];
    int i, index;
    printf("\nInserta el nombre de la interfaz: ");
    gets(nombre);
    strcpy(nic.ifr_name, nombre);
    if (ioctl(ds, SIOCGIFINDEX, &nic) == -1)
    {
        perror("\nError al obtener el index");
        exit(0);
    }
    else
    {
        index = nic.ifr_ifindex;
        printf("\nEl indice es: %d", index);
        if (ioctl(ds, SIOCGIFHWADDR, &nic) == -1)
        {
            perror("\nError al obtener la MAC.");
            exit(0);
        }
        else
        {
            memcpy(MACorigen, nic.ifr_hwaddr.sa_data + 0, 6);
            printf("\nLa MAC es: ");
            for (i = 0; i < 6; i++)
            {
                printf("%.2x:", MACorigen[i]);
            }
        }
        if (ioctl(ds, SIOCGIFADDR, &nic) == -1)
        {
            perror("\nError al obtener la IP.");
            exit(0);
        }
        else
        {
            memcpy(ip, nic.ifr_addr.sa_data + 2, 4);
            printf("\nLa IP es: ");
            for (i = 0; i < 4; i++)
            {
                printf("%d.", ip[i]);
            }
        }
        if (ioctl(ds, SIOCGIFNETMASK, &nic) == -1)
        {
            perror("\nError al obtener la mascara.");
            exit(0);
        }
        else
        {
            memcpy(mascara, nic.ifr_netmask.sa_data + 2, 4);
            printf("\nLa mascara es: ");
            for (i = 0; i < 4; i++)
            {
                printf("%d.", mascara[i]);
            }
            printf("\n");
        }
    }
    return index;
}
void estructuraTrama(unsigned char *trama)
{
    memcpy(trama + 0, MACbroad, 6);
    memcpy(trama + 6, MACorigen, 6);
    memcpy(trama + 12, ethertype, 2);
    memcpy(trama + 14, "Carlos Eduardo Velasco", 22);
}
void enviarTrama(int ds, int index, unsigned char *trama)
{
    int tam;
    struct sockaddr_ll interfaz;
    memset(&interfaz, 0x00, sizeof(interfaz));
    interfaz.sll_family = AF_PACKET;          /* Always AF_PACKET */
    interfaz.sll_protocol = htons(ETH_P_ALL); /* Physical-layer protocol */
    interfaz.sll_ifindex = index;             /* Physical-layer protocol */
    tam = sendto(ds, trama, 60, 0, (struct sockaddr *)&interfaz, sizeof(interfaz));
    if (tam == -1)
    {
        perror("\nError al enviar");
        exit(0);
    }
    else
    {
        perror("\nExito al enviar");
    }
}
int main()
{
    int packet_socket, indice;
    packet_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (packet_socket == -1)
    {
        perror("\nError al abrir el socket");
        exit(0);
    }
    else
    {
        perror("\nExito al abrir el socket");
        indice = obtenerDatos(packet_socket);
        estructuraTrama(tramaEnv);
        enviarTrama(packet_socket, indice, tramaEnv);
    }
    close(packet_socket);
    return 0;
}