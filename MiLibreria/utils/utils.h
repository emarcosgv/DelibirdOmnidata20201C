#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>

typedef enum {
	SUSCRIPCION = 1, MENSAJE
} t_opCode;

//por ahi se puede unificar t_colas y t_mensajes por ahora estan separados
// esto se usa en broker,pero se puede usar en cualquier lugar para enumerar el switch y tener un protocolo comun
typedef enum t_colaMensaje {
	tNEW_POKEMON = 1,
	tAPPEARED_POKEMON,
	tCATCH_POKEMON,
	tCAUGHT_POKEMON,
	tGET_POKEMON,
	tLOCALIZED_POKEMON,

	tFinDeProtocolo //NO SACAR Y DEJAR A LO ULTIMO!!!
} t_colaMensaje;

typedef struct {
	uint32_t id;
	uint32_t idCorrelacional;
} t_mensaje;

//este es buffer original de NEW_POKEMON
/*
 typedef struct {
 int size;
 void* stream;

 int posX;
 int posY;
 int cantidadPokemons;
 } t_buffer;
 */
typedef struct {
	int size;
	void* stream;

	//int booleano;
	int posX;
	int posY;
	int cantidadPokemons;
} t_buffer;

typedef struct {
	int x;
	int y;
} t_posicion;
typedef struct {
	t_buffer* buffer;
	t_colaMensaje colaMensaje;
	t_opCode codigoOperacion;
	int pid;
	int bytes;
} t_paquete;

//typedef struct {
////	char *ip;
////	int puerto;
//	int pid;
//	t_opCode codigoOperacion;
//	t_colaMensaje colaMensaje;
//} t_suscriptor;

void* serializarPaqueteNewPokemon(t_paquete* paquete, int bytes);
int crearConexion(char *ip, int puerto, int tiempo_reconexion);
void enviarMensaje(char *mensaje, int socket);
void liberarConexion(int socket);
t_paquete *recibirMensajeNewPokemon(int socket_cliente, int* size);

//void crearMensaje(void* payload, int socket_cliente);
void crearMensajeNewPokemon(int pid, char* nombrePokemon, int posX, int posY,
		int cantidadPokemons, int socket_cliente);

void devolverMensajeConfirmacion(void* layout, int socket_cliente);
char* recibirConfirmacion(int socket_cliente);

t_paquete *recibirSuscriptor(int socket_cliente, int* size);
void* serializarSuscriptor(t_paquete* suscriptor, int bytes);
void suscribirseAcola(int pid, char* nombreCola, int socket);

t_paquete *recibirMensajeGetPokemon(int socket_cliente, int* size);
void crearMensajeGetPokemon(int pid, char* nombrePokemon, int socket_cliente);
void* serializarPaqueteGetPokemon(t_paquete* paquete, int bytes);

t_paquete *recibirMensajeAppearedPokemon(int socket_cliente, int* size);
void* serializarPaqueteAppearedPokemon(t_paquete* paquete, int bytes);
void crearMensajeApperedPokemon(int pid, char* nombrePokemon, int posX,
		int posY, int socket_cliente);

t_paquete *recibirMensajeCatchPokemon(int socket_cliente, int* size);
void* serializarPaqueteCatchPokemon(t_paquete* paquete, int bytes);
void crearMensajeCatchPokemon(int pid, char* nombrePokemon, int posX, int posY,
		int socket_cliente);

t_paquete *recibirMensajeCaughtPokemon(int socket_cliente, int* size);
void* serializarPaqueteCaughtPokemon(t_paquete* paquete, int bytes);
void crearMensajeCaughtPokemon(int pid, int booleano, int socket_cliente);

t_paquete *recibirMensajeLocalizedPokemon(int socket_cliente, int* size);
void* serializarPaqueteLocalizedPokemon(t_paquete* paquete, int bytes);
void crearMensajeLocalizedPokemon(int pid, char* nombrePokemon, int posX,
		int posY, int cantidadPokemons, int socket_cliente);

t_paquete *recibirMensaje(int socketCliente, int* size);

#endif/*UTILS_UTILS_H*/
