#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sqlite3.h"

#define CREATE_TABLA_REGISTROS "CREATE TABLE IF NOT EXISTS registros(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, ip TEXT NOT NULL, mac TEXT NOT NULL)"

typedef struct Registro Registro;

struct Registro{
	char ip[16];
	char mac[18];
	Registro *next;
};

void mostrarLista(Registro *lista){
	if(lista == NULL){
		printf("Vacía xd\n");
		return;
	}
	
	Registro *aux = lista;
	while(aux->next != NULL){
		printf("\nIP: %s\nMAC: %s\n", aux->ip, aux->mac);
		aux = aux->next;
	}
}

Registro *registroNuevo(Registro *registro){
	Registro *r = (Registro *) malloc(sizeof(Registro));
	
	strcpy(r->ip, registro->ip);
	strcpy(r->mac, registro->mac);
	r->next = NULL;
	
	return r;
}

void agregarRegistro(Registro **lista, Registro *registro){
	if(lista == NULL)
		return;
		
	if(*lista == NULL){
		*lista = registroNuevo(registro);
		return;
	}
	
	Registro *aux = *lista;
	
	while(aux->next != NULL)
		aux = aux->next;
		
	aux->next = registroNuevo(registro);
}

int gestionarError(sqlite3 *db){
	fprintf(stderr, "ERROR: %s\n", sqlite3_errmsg(db));
	return sqlite3_errcode(db);
}

int main(){
	Registro *lista = NULL, registro;
	sqlite3 *db = NULL;
	const char *filename = "/home/eduardo/Escritorio/Redes/escanerxd/registro.db";
	
	//Abre BD
	if(sqlite3_open(filename, &db) != SQLITE_OK)
		return gestionarError(db);
	
	//Configura BD
	if(sqlite3_exec(db, CREATE_TABLA_REGISTROS, NULL, NULL, NULL) != SQLITE_OK)
		return gestionarError(db);
	
	strcpy(registro.ip, "1.1.1.1");
	strcpy(registro.mac, "ff:ff:ff:ff:ff:ff");
	agregarRegistro(&lista, &registro);
	strcpy(registro.ip, "192.1.2.4");
	strcpy(registro.mac, "aa:aa:aa:aa:aa:aa");
	agregarRegistro(&lista, &registro);
	strcpy(registro.ip, "255.255.255.255");
	strcpy(registro.mac, "cc:cd:0a:ad:f1:b1");
	agregarRegistro(&lista, &registro);
	
	mostrarLista(lista);
	
	return 0;
}
