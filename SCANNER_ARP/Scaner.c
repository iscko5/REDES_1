/*    MANUALES
  man 7 packet
  man netdevice
  man send to
  man recvfrom
*/

#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

unsigned char MACOrigen[6];
unsigned char IPOrigen[4];
unsigned char IPDestino[4] = {192, 168, 100, 0};
unsigned char MaskOrigen[4];
unsigned char MACBrdCst[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
unsigned char EthertypeARP[2] = {0x08, 0x06};
unsigned char TipHD[2] = {0x00, 0x01};
unsigned char TipProt[2] = {0x08, 0x00};
unsigned char LngDrHd[1] = {0x06};
unsigned char LngDrPr[1] = {0x04};
unsigned char CodOpr[2] = {0x00, 0x01};

void obtenerDatos(int, int *);
void crearTrama(unsigned char *);
void enviarTrama(int, int, unsigned char *);
void recibirTrama(int, unsigned char *);
void ImpTrama(unsigned char *, int);
void ImpMac(unsigned char *);

int main(int argc, char const *argv[])
{

  unsigned char TramaEnv[1514];
  unsigned char TramaRec[1514];
  int packet_socket;
  int Index;

  packet_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

  //-----------------------------------------------------------SECCION PARA ABRIR EL SOCKET
  if (packet_socket == -1)
  {
    perror("\nError al abrir el socket ");
    exit(0);
  }
  else
  {
    // perror("\nExito al abrir el socket ");
    printf("\n\t\t ESCANEANDO");
    obtenerDatos(packet_socket, &Index);
    // printf("EL index1 es: %d\n", Index);
    printf("\n\tIP\t --------------\t\tMAC\n");
    for (int i = 0; i < 255; i++)
    {
      IPDestino[3] = i;
      crearTrama(TramaEnv);
      enviarTrama(packet_socket, Index, TramaEnv);
      recibirTrama(packet_socket, TramaRec);
    }
    // puts(TramaEnv);
  }
  printf("\n");
  close(packet_socket);

  return 0;
}

void obtenerDatos(int ds, int *Indice)
{
  struct ifreq nic;
  unsigned char nombre[20] = {'w', 'l', 'o', '1'}; // DEJAMOS POR DEFAULT LA INTERFAZ WIFI

  /// printf("Inserta el nombre de la interfaz: \n");
  /// gets(nombre);
  strcpy(nic.ifr_name, nombre);
  /*printf("\nIngrese la IP de destino: \n");
  fflush(stdin);
  for (int i = 0; i < 4; i++){
    scanf(" %hhu", &IPDestino[i]);
  }*/

  //-----------------------------------------------------------SECCION PARA OBTENER EL INDEX DE LA INTERFAZ
  if (ioctl(ds, SIOCGIFINDEX, &nic) == -1)
  {
    perror("Error al obtener el index ");
    exit(0);
  }
  else
  {
    (*Indice) = nic.ifr_ifindex;
    // printf("\nEl indice es: %d\n",nic.ifr_ifindex);

    //-----------------------------------------------------------SECCION PARA OBTENER LA MAC
    if (ioctl(ds, SIOCGIFHWADDR, &nic) == -1)
    {
      perror("Error al obtener la MAC ");
      exit(0);
    }
    else
    {
      memcpy(MACOrigen, nic.ifr_hwaddr.sa_data, 6);
      // printf("La MAC es: \n");

      /*for (int i = 0; i < 6; i++) {
        printf("%.2x:", MACOrigen[i]);
      }
      printf("\n");*/
    }

    //-----------------------------------------------------------SECCION PARA OBTENER LA IP
    if (ioctl(ds, SIOCGIFADDR, &nic) == -1)
    {
      perror("Error al obtener la IP ");
      exit(0);
    }
    else
    {
      memcpy(IPOrigen, nic.ifr_addr.sa_data + 2, 4);
      /* printf("la IP es: \n");

       for (int i = 0; i < 4; i++) {
         printf("%d.", IPOrigen[i]);
       }
       printf("\n");*/
    }

    //-----------------------------------------------------------SECCION PARA OBTENER LA MASCARA
    if (ioctl(ds, SIOCGIFNETMASK, &nic) == -1)
    {
      perror("Error al obtener la Mascara ");
      exit(0);
    }
    else
    {
      memcpy(MaskOrigen, nic.ifr_netmask.sa_data + 2, 4);
      /*printf("la Mascara es: \n");

      for (int i = 0; i < 4; i++) {
        printf("%d.", MaskOrigen[i]);
      }
      printf("\n");*/
    }
  }
}
//-----------------------------------------------------------SECCION PARA CREAR LA TRAMA

void crearTrama(unsigned char *trama)
{ // TRAMA PARA PROTOCOLO ARP
  // ENCABEZADO MAC
  memcpy(trama + 0, MACBrdCst, 6);
  memcpy(trama + 6, MACOrigen, 6);
  memcpy(trama + 12, EthertypeARP, 2); // MENSAJE ARP
  // memcpy(trama + 14, "Marco Antonio Ortega Flores", 27);
  memcpy(trama + 14, TipHD, 2);     // TIPO HARDWARE
  memcpy(trama + 16, TipProt, 2);   // TIPO PROTOCOLO
  memcpy(trama + 18, LngDrHd, 1);   // LONG. DIR. HARDWARE
  memcpy(trama + 19, LngDrPr, 1);   // LONG. DIR. PROTOCOLO
  memcpy(trama + 20, CodOpr, 2);    // CODIGO DE OPERACION
  memcpy(trama + 22, MACOrigen, 6); // DIR. HARDWARE ORIGEN
  memcpy(trama + 28, IPOrigen, 4);  // DIR. PROTOCOLO ORIGEN
  memcpy(trama + 32, MACBrdCst, 6); // DIR. HARDWARE DESTINO
  memcpy(trama + 38, IPDestino, 4); // DIR. PROTOCOLO DESTINO
}
//-----------------------------------------------------------SECCION PARA ENVIAR LA TRAMA

void enviarTrama(int ds, int Index, unsigned char *trama)
{
  struct sockaddr_ll interfaz;
  int tam;

  memset(&interfaz, 0x00, sizeof(interfaz));

  interfaz.sll_family = AF_PACKET;
  interfaz.sll_protocol = htons(ETH_P_ALL);
  interfaz.sll_ifindex = Index;

  tam = sendto(ds, trama, 60, 0, (struct sockaddr *)&interfaz, sizeof(interfaz));
  if (tam == -1)
  {
    perror("Error al Enviar ");
    exit(0);
  }
  else
  {
    // perror("Exito al Enviar ");
    // ImpTrama(trama, tam);
  }
}

void recibirTrama(int ds, unsigned char *trama)
{
  int tam, bandera = 0;
  struct timeval start, end;
  long mtime = 0, seconds, useconds;

  printf("\n");

  gettimeofday(&start, NULL);

  for (int i = 0; i < 4; i++)
  { // IMPRESION DE LA IP SIN IMPORTAR EL RESULTADO
    printf("%d.", IPDestino[i]);
  }
  printf("  --------------  ");

  while (mtime < 200)
  { // LIMITE DE TIEMPI EN MLS PARA RECIBIR UNA RESPUESTA
    tam = recvfrom(ds, trama, 1514, 0, NULL, 0);

    if (tam == -1)
    {
      perror("\nError al Recibir");
      exit(0);
    }
    else
    {
      if (!memcmp(trama + 0, MACOrigen, 6) || !memcmp(trama + 6, MACOrigen, 6))
      {

        if (!memcmp(trama + 12, EthertypeARP, 2))
        { // COMPROBAMOS PROTOCOLO ARP
          // ImpTrama(trama, tam);
          if (trama[21] == 2)
          { // COMPROBAMOS QUE SEA UNA RESPUESTA
            // ImpTrama(trama, tam);
            // usleep(100);
            ImpMac(trama);
            // printf("\n");
            bandera = 1;
          }
        }
        // break;      //PARA RECIBRIR SOLO UNA TRAMA
      }
      gettimeofday(&end, NULL);
      seconds = end.tv_sec - start.tv_sec;
      useconds = end.tv_usec - start.tv_usec;
      mtime = ((seconds)*1000 + useconds / 1000.0) + 0.5; // TOTAL DE MLS TRANSCURRIDOS

      if (bandera == 1)
      {
        break;
      }
    }
  }
  // printf("Elapsed time: %ld milliseconds\n\n", mtime);
}

void ImpTrama(unsigned char *trama, int len)
{ // FUNCION PARA IMPRIMIR LA TRAMA
  int i;

  for (i = 0; i < len; i++)
  {
    if (i % 16 == 0)
      printf("\n");
    printf(" %.2x", trama[i]);
  }
  printf("\n");
}

void ImpMac(unsigned char *trama)
{ // FUNCION PARA IMPRIMIR LA MAC DESDE LA TRATA RECIBIDA

  for (int i = 6; i < 12; i++)
  {
    printf("%.2x:", trama[i]);
  }
  // printf("\n");
}