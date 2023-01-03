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
unsigned char IPorigen[4];
unsigned char tramaARPsol[60] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x06, 0x00, 0x01,
                                 0x08, 0x00, 0x06, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 'I', 'S', 'V'};

void estructuraARPsol(unsigned char *trama)
{
    // Encabezado MAC
    memcpy(trama + 6, MACorigen, 6);
    // Mensaje de ARP
    memcpy(trama + 22, MACorigen, 6);
    memcpy(trama + 28, IPorigen, 4);
    memset(trama + 32, 0x00, 6);
    memcpy(trama + 38, IPdestino, 4);
}

void enviaTrama(int ds, int index, unsigned char *paq)
{
    int tam;
    struct sockaddr_ll capaEnlace;
    memset(&capaEnlace, 0x00, sizeof(capaEnlace));
    capaEnlace.sll_family = AF_PACKET;
    capaEnlace.sll_protocol = htons(ETH_P_ALL);
    capaEnlace.sll_ifindex = index;
    tam = sendto(ds, paq, 60, 0, (struct sockaddr *)&capaEnlace, sizeof(capaEnlace));
    if (tam == -1)
        perror("\nError al enviar la trama");
    else
        perror("\nExito al enviar la trama");
}

void imprimeTrama(unsigned char *trama, int tam)
{
    int i, j;
    for (i = 1; i <= tam; i++)
    {
        printf("%.2x ", trama[i - 1]);
        if (i % 16 == 0)
            printf("\n");
    }
    printf("\n");
}

void recibeARPresp(int ds, unsigned char *trama)
{
    int tam, bandera = 0;
    while (1)
    {
        tam = recvfrom(ds, trama, 1514, 0, NULL, 0);
        if (tam == -1)
            perror("\nError al recibir");
        else
        {
            bandera = filtroARP(trama, tam);
            if (bandera == 1)
            {
                break;
            }
        }
    }
}

int filtroARP(unsigned char *paq, int len)
{
    if (!memcmp(paq, MACorigen, 6))
    {
        if (!memcmp(paq + 12, etherARP, 2))
            if (!memcmp(paq + 20, codARPresp, 2))
                if (!memcmp(paq + 38, IPorigen, 4))
                    return 1;
    }
    else
        return 0;
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
        perror("Exito al abrir el socket");
        indice = obtenDatos(packet_socket);
        obtenIPdestino();
        estructuraARPsol(tramaARPsol);
        printf("\n**********La trama que se envia es*************\n");
        imprimeTrama(tramaARPsol, 60);
        enviaTrama(packet_socket, indice, tramaARPsol);

        recibeARPresp(packet_socket, tramaARPresp);
        printf("\n**********La trama que se recibe de respuesta ARP es*************\n");
        imprimeTrama(tramaARPresp, 60);
    }
    close(packet_socket);
    return 1;
}
