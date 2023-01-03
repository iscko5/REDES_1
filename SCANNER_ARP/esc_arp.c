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

unsigned char trama[1514];
unsigned char macDest[6]={0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char macOr[6];//Obtener segun la interfaz de red
unsigned char ethertype[2]={0x08, 0x06};
unsigned char hw[2]={0x00, 0x01};
unsigned char prot[2]={0x08, 0x00};
unsigned char longDirHw=0x06;
unsigned char longDirProt=0x04;
unsigned char codeOp[2]={0x00, 0x01};
unsigned char ipOr[4];//A definir segun la interfaz de red
unsigned char ipDest[4]={192, 168, 0, 0};//Capturar

unsigned char macBrd[6]={0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
unsigned char codeOpResp[2]={0x00, 0x02};
unsigned char macResp[6];


int filtroARP(unsigned char * trama){
	if(!memcmp(trama+12, ethertype, 2) && !memcmp(trama+20, codeOpResp, 2) && 
	   !memcmp(trama+0, macOr, 6) && !memcmp(trama+32, macOr, 6) &&
	   !memcmp(trama+38, ipOr, 4) && !memcmp(trama+28, ipDest, 4))
		return 1;	   
	else
		return 0;
}

int obtenerDatos(int ds){
	struct ifreq nic;
	unsigned char nombre[10];
	int index;
	
	printf("\nInserta el NOMBRE de la interfaz de red a utilizar: ");
	fgets(nombre, 10, stdin);
	strtok(nombre, "\n");
	strcpy(nic.ifr_name, nombre);

	if(ioctl(ds, SIOCGIFINDEX, &nic)==-1){
		perror("\nERROR al obtener index");
		exit(0);
	}else{//interfaz obtenida
		index=nic.ifr_ifindex;
		printf("\nEl indice es: %d", index);

		if(ioctl(ds, SIOCGIFHWADDR, &nic)==-1){//obtener mac uwu
			perror("\nError al obtener la MAC");
			exit(0);
		}else//Mac obtenida
			memcpy(macOr, nic.ifr_hwaddr.sa_data, 6);//mac copiada

		if(ioctl(ds, SIOCGIFADDR, &nic)==-1){//obtener IP uwu
			perror("\nError al obtener la IP");
			exit(0);
		}else//IP obtenida
			memcpy(ipOr, nic.ifr_addr.sa_data+2, 4);//ip copiada
			
		printf("\n");
	}
	return index;
}

void estructuraTramaSolARP(unsigned char * trama){
	//Encabezado MAC
	memcpy(trama+0, macBrd,6);
	memcpy(trama+6, macOr, 6);
	memcpy(trama+12, ethertype, 2);
	
	//Mensaje ARP
	memcpy(trama+14, hw, 2);
	memcpy(trama+16, prot, 2);
	trama[18]=longDirHw;
	trama[19]=longDirProt;
	memcpy(trama+20, codeOp, 2);
	memcpy(trama+22, macOr, 6);
	memcpy(trama+28, ipOr, 4);
	memcpy(trama+32, macDest, 6);
	memcpy(trama+38, ipDest, 4);
}

int recibirTrama(int ds, unsigned char * trama){
	int tam=0, bandera=0;
	long mtime=0, seconds, useconds;  
	struct timeval start, end;

	gettimeofday(&start, NULL);
	
    	while(mtime<500){
		tam=recvfrom(ds, trama, 1514, MSG_DONTWAIT, NULL, 0);
		if(tam!=-1)
			bandera=filtroARP(trama);
		
		gettimeofday(&end, NULL);
    		seconds  = end.tv_sec  - start.tv_sec;
	    	useconds = end.tv_usec - start.tv_usec;
    		mtime = ((seconds) * 1000 + useconds/1000.0);
    	
    		if(bandera==1)
  			break;
	}
	
	return tam;
}

void enviarTrama(int ds, int index, unsigned char * trama){
	int tam;
	struct sockaddr_ll interfaz;
	memset(&interfaz, 0x00, sizeof(interfaz));
	
	interfaz.sll_family=AF_PACKET;
	interfaz.sll_protocol=htons(ETH_P_ALL);
	interfaz.sll_ifindex=index;
	
	tam=sendto(ds, trama, 42, 0, (struct sockaddr *)&interfaz, sizeof(interfaz));
	
	if(tam==-1){
		perror("\nERROR al ENVIAR TRAMA");
		exit(0);
	}	
}

void imprimirRespuesta(){
	int i;
	
	printf("\nIP: ");
	for(i=0; i<4; i++)
		printf("%d:", ipDest[i]);
	printf("\nMAC: ");
	for(i=0; i<6; i++)
		printf("%.2x:", macResp[i]);
}

void escaneo(int ds, int index){
	int i, resp;
	
	for(i=0; i<255; i++){
		ipDest[3]=i;
		estructuraTramaSolARP(trama);
		enviarTrama(ds, index, trama);
		resp=recibirTrama(ds, trama);
		
		if(resp!=-1){//Cuando haya respuesta :p
			memcpy(macResp, trama+6, 6);
			printf("\n");
			imprimirRespuesta();
		}
	}
}

int main(){
	int packet_socket, indice;
	packet_socket=socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	
	if(packet_socket==-1){
		perror("\nERROR al abrir el socket");
		exit(0);
	}else{
		perror("\nEXITO al abrir el socket");
		indice=obtenerDatos(packet_socket);//Obteniendo indice
		printf("\n----*INICIANDO ESCANEO ARP :3*----");
		escaneo(packet_socket, indice);
		printf("\n\n----*ESCANEO FINALIZADO :P*----");
	}	
	close(packet_socket);
	return 0;
}
