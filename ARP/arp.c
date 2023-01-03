#include <linux/if_packet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <net/ethernet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>

unsigned char MACorigen[6];
unsigned char IPorigen[4];
unsigned char MASKorigen[4];
unsigned char MACbroadcast[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
unsigned char ethertype[2] = {0x08, 0x06};
unsigned char tramaEnv[1514];
unsigned char tramaRec[1514];
unsigned char proto[2] = {0x08, 0x00};
unsigned char hw[2] = {0x00, 0x01};
unsigned char opcode[2] = {0x00, 0x01};

unsigned char ipor[4];
unsigned char IP[50];

int obtenerdatos(int ds)
{
    struct ifreq nic;
    int i, index;
    unsigned char nombre[20];
    printf("\n Inserta el nombre de la interfaz\n");
    gets(nombre);
    printf("\n Inserta la ip\n");
    gets(IP);
    inet_aton(IP, ipor);
    strcpy(nic.ifr_name, nombre);
    if (ioctl(ds, SIOCGIFINDEX, &nic) == -1)
    {
        perror("\nError al obtener el socket");
        exit(0);
    }
    else
    {
        index = nic.ifr_ifindex;
        printf("\nEl indice es %d  ", index);
        if (ioctl(ds, SIOCGIFHWADDR, &nic) == -1)
        {
            perror("\nError al obtener la MAC ");
            exit(0);
        }
        else
        {
            memcpy(MACorigen, nic.ifr_hwaddr.sa_data, 6);
            printf("\nLa MAC es: ");
            for (i = 0; i < 6; i++)
            {
                printf("%.2x:", MACorigen[i]);
            }
        }
        if (ioctl(ds, SIOCGIFADDR, &nic) == -1)
        {
            perror("\nError al obtener la ip ");
            exit(0);
        }
        else
        {
            memcpy(IPorigen, nic.ifr_addr.sa_data + 2, 4);
            printf("\nLa IP es: ");
            for (i = 0; i < 4; i++)
            {
                printf("%d.", IPorigen[i]);
            }
            if (ioctl(ds, SIOCGIFNETMASK, &nic) == -1)
            {
                perror("\nError al obtener la NetMask ");
                exit(0);
            }
            else
            {
                memcpy(MASKorigen, nic.ifr_netmask.sa_data + 2, 4);
                printf("\nLa NetMask es: ");
                for (i = 0; i < 4; i++)
                {
                    printf("%d.", MASKorigen[i]);
                }
            }
        }
    }
    return index;
}

void estructuraTrama(unsigned char *trama)
{
    memcpy(trama + 0, MACbroadcast, 6);
    memcpy(trama + 6, MACorigen, 6);
    memcpy(trama + 12, ethertype, 2);
    memcpy(trama + 14, hw, 2);
    memcpy(trama + 16, proto, 2);
    trama[18] = 6;
    trama[19] = 4;
    memcpy(trama + 20, opcode, 2);
    memcpy(trama + 22, MACorigen, 6);
    memcpy(trama + 28, IPorigen, 4);
    memset(trama + 32, 0x00, 6);
    memcpy(trama + 38, ipor, 4);
    memcpy(trama + 43, "ISAAC", 6);
}

void enviarTrama(int ds, int index, unsigned char *trama)
{
    int tam;
    struct sockaddr_ll interfaz;
    memset(&interfaz, 0x00, sizeof(interfaz));
    interfaz.sll_family = AF_PACKET;
    interfaz.sll_protocol = htons(ETH_P_ALL);
    interfaz.sll_ifindex = index;

    tam = sendto(ds, trama, 60, 0, (struct sockaddr *)&interfaz, sizeof(interfaz));
    if (tam == -1)
    {
        perror("\nError al enviar");
        exit(0);
    }
    else
        perror("\nExito al enviar");
}

void imprimirTrama(unsigned char *paq, int len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        if (i % 16 == 0)
            printf("\n");
        printf("%.2x ", paq[i]);
    }
}

void recibirTrama(int ds, unsigned char *trama)
{
    int tam;
    struct timeval start, end;
    long mtime = 0, seconds, useconds;
    gettimeofday(&start, NULL);
    while (mtime < 200)
    {
        tam = recvfrom(ds, trama, 1514, MSG_DONTWAIT, NULL, 0);
        if (tam == -1)
        {
            perror("\nError al recibir");
        }
        else if (!memcmp(trama + 0, MACorigen, 6) || !memcmp(trama + 6, MACorigen, 6))
        {
            imprimirTrama(trama, tam);
            break;
        }

        gettimeofday(&end, NULL);
        seconds = end.tv_sec - start.tv_sec;
        useconds = end.tv_usec - start.tv_usec;
        mtime = ((seconds)*1000 + useconds / 1000.0) + 0.5;
    }

    printf("Elapsed Time: %ld milisegundos\n", mtime);
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
        indice = obtenerdatos(packet_socket);
        estructuraTrama(tramaEnv);
        enviarTrama(packet_socket, indice, tramaEnv);
        erecibirTrama(packet_socket, tramaRec);
        exit(0);
    }
    close(packet_socket);
    return 0;
}
