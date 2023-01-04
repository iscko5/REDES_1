#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/time.h>

unsigned char tramaSol[1514];
unsigned char tramaRes[1514];
unsigned char macBrd[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
unsigned char macOr[6];
unsigned char macDest[6];
unsigned char ethertype[2] = {0x08, 0x06};
unsigned char tipoHw[2] = {0x00, 0x01};
unsigned char prot[2] = {0x08, 0x00};
unsigned char logDirHw = 0x06;
unsigned char logDirProt = 0x04;
unsigned char codeOp[2];
unsigned char ipOr[4];
unsigned char ipDest[4];

int obtenerIndice(int ds)
{
	struct ifreq nic;
	unsigned char nombre[10];
	int index;

	printf("\nInserta el NOMBRE de la interfaz: ");
	fgets(nombre, 10, stdin);
	strtok(nombre, "\n");
	strcpy(nic.ifr_name, nombre);

	if (ioctl(ds, SIOCGIFINDEX, &nic) == -1)
	{
		perror("\nERROR al obtener index");
		exit(0);
	}
	else
	{ // interfaz obtenida
		index = nic.ifr_ifindex;
		printf("\nEl indice es: %d", nic.ifr_ifindex);

		if (ioctl(ds, SIOCGIFHWADDR, &nic) == -1)
		{ // obtener mac uwu
			perror("\nError al obtener la MAC");
			exit(0);
		}
		else
		{											  // Mac obtenida
			memcpy(macOr, nic.ifr_hwaddr.sa_data, 6); // mac copiada

			printf("\nLa MAC es: ");
			for (int i = 0; i < 6; i++)
				printf("%.2x:", macOr[i]);
		}

		if (ioctl(ds, SIOCGIFADDR, &nic) == -1)
		{ // obtener IP uwu
			perror("\nError al obtener la IP");
			exit(0);
		}
		else
		{											   // IP obtenida
			memcpy(ipOr, nic.ifr_addr.sa_data + 2, 4); // ip copiada

			printf("\nLa IP es: ");
			for (int i = 0; i < 4; i++)
				printf("%d:", ipOr[i]);
		}
		printf("\n");
	}
	return index;
}

void estructuraTramaSol(unsigned char *trama)
{
	memcpy(trama + 0, macBrd, 6);
	memcpy(trama + 6, macOr, 6);
	memcpy(trama + 12, ethertype, 2);
	memcpy(trama + 14, "Lalo UwU", 8);
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
		perror("\nERROR al ENVIAR TRAMA");
		exit(0);
	}
	else
	{
		perror("\nEXITO al ENVIAR trama");
	}
}

void imprimirTrama(unsigned char *trama, int len)
{
	for (int i = 0; i < len; i++)
	{
		if (i % 16 == 0)
			printf("\n");
		printf("%.2x ", trama[i]);
	}
	printf("\n");
}

void recibirTrama(int ds, unsigned char *trama)
{
	int tam, bandera = 0;
	struct timeval start, end;

	long mtime = 0, seconds, useconds;

	gettimeofday(&start, NULL);
	while (mtime < 200)
	{
		tam = recvfrom(ds, trama, 1514, MSG_DONTWAIT, NULL, 0);
		if (tam == -1)
		{
			perror("\nERROR al RECIBIR trama");
		}
		else
		{
			if (!memcmp(trama + 0, macOr, 6) || !memcmp(trama + 26, macOr, 4))
			{ //(!memcmp(trama + 0, MACorigen, 6) || !memcmp(trama + 6, MACorigen, 6))
				imprimirTrama(trama, tam);
				bandera = 1;
			}
		}
		gettimeofday(&end, NULL);

		seconds = end.tv_sec - start.tv_sec;
		useconds = end.tv_usec - start.tv_usec;
		mtime = ((seconds)*1000 + useconds / 1000.0) + 0.5;
		if (bandera == 1)
			break;
	}
	printf("Elapsed time: %ld milliseconds\n", mtime);
}

void obtenerIpDestino()
{
}

int main()
{
	int packet_socket, indice;
	packet_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

	if (packet_socket == -1)
	{
		perror("\nERROR al abrir el socket");
		exit(0);
	}
	else
	{
		perror("\nEXITO al abrir el socket");
		/*Obteniendo Indice, ip Origen, mac Origen*/
		indice = obtenerIndice(packet_socket);
		// estructuraTrama(tramaEnv);
		// enviarTrama(packet_socket, indice, tramaEnv);
		recibirTrama(packet_socket, tramaRes);
	}

	close(packet_socket);
	return 0;
}
