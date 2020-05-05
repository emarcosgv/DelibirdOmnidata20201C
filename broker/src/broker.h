#ifndef BROKER_BROKER_H_
#define BROKER_BROKER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
//#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/error.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>
#include "../MiLibreria/utils/servidor.h"
#include "../MiLibreria/utils/utils.h"
#include <pthread.h>
#include <sys/types.h>
//#include "../sockets/servidor.h"
//#include "../bibliotecasPropias/serializacion/serializacion.h"
//#include "../bibliotecasPropias/sockets/sockets.h"

#define BROKER_CONFIG_PATH "broker.config"

typedef struct {
	int tamanoMemoria;
	int tamanoMinimoParticion;
	char *algoritmoMemoria;
	char *algoritmoReemplazo;
	char *ipBroker;
	int puertoBroker;
	int frecuenciaCompactacion;
	char *logFile;

} t_BROKERConfig;

typedef struct {
	t_queue *cola;
	t_list *lista;
} t_cola;

// ver que se necesita para el suscriptor, como manejar la cola a la que quiere suscribirse



t_log *logger;
t_BROKERConfig *brokerConf;
t_config *BROKERTConfig;

t_cola *NEW_POKEMON;
t_cola *APPEARED_POKEMON;
t_cola *CATCH_POKEMON;
t_cola *CAUGTH_POKEMON;
t_cola *GET_POKEMON;
t_cola *LOCALIZED_POKEMON;

typedef struct {
	t_cola *cola1;
	t_cola *cola2;
} t_parejaCola;

t_parejaCola *NEW_APPEARED_POKEMON;
t_parejaCola *CATCH_CAUGTH_POKEMON;
t_parejaCola *GET_LOCALIZED_POKEMON;

void inicializarLogger(void);
void cargarConfigBROKER(void);
void inicializarColasBroker(void);
void destruirColasBroker(void);
void agregarMensaje(t_cola*, void*);
char* sacarMensaje(t_cola*);
//void administrarSuscriptores(t_suscriptor suscriptor);
void administrarColas(void* stream,void* clienteFd);
void pedirMemoriaInicial(void);
//void* handler(void* socketConectado);
//int createSocket(void);
//void esperarCliente(int);
void processRequest(void* bufferLoco, int clienteFd);
void *serveClient(void *socket);

void* handler(void* socketConectado);

#endif /* BROKER_BROKER_H_ */
