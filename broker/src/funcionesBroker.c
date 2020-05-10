#include "broker.h"

void inicializarLogger() {
	//Crea el logger
	logger = log_create("BROKER.log", "BROKER", 1, LOG_LEVEL_TRACE);
}

void cargarConfigBROKER() {
	//printf("pude cargar la configuracion correctamente");
	//Carga la configuracion del txt de config al struct de config

	//brokerConf = (t_BROKERConfig)malloc(sizeof(t_BROKERConfig));
	brokerConf = (t_BROKERConfig*) malloc(sizeof(t_BROKERConfig));
	BROKERTConfig = config_create(BROKER_CONFIG_PATH);
	//inicializarLogger();
	if (BROKERTConfig == NULL) {
		perror("error archivo de configuracion");
		log_error(logger, "- NO SE PUDO IMPORTAR LA CONFIGURACION");
		exit(1);
	}

	brokerConf->tamanoMemoria = config_get_int_value(BROKERTConfig,
			"TAMANO_MEMORIA");
	log_info(logger, "TAMANO_MEMORIA=%d", brokerConf->tamanoMemoria);
	brokerConf->tamanoMinimoParticion = config_get_int_value(BROKERTConfig,
			"TAMANO_MINIMO_PARTICION");
	log_info(logger, "TAMANO_MINIMO_PARTICION=%d",
			brokerConf->tamanoMinimoParticion);
	brokerConf->algoritmoMemoria = string_duplicate(
			config_get_string_value(BROKERTConfig, "ALGORITMO_MEMORIA"));
	log_info(logger, "ALGORITMO_MEMORIA=%s", brokerConf->algoritmoMemoria);
	brokerConf->algoritmoReemplazo = string_duplicate(
			config_get_string_value(BROKERTConfig, "ALGORITMO_REEMPLAZO"));
	log_info(logger, "ALGORITMO_REEMPLAZO=%s", brokerConf->algoritmoReemplazo);
	brokerConf->ipBroker = string_duplicate(
			config_get_string_value(BROKERTConfig, "IP_BROKER"));
	log_info(logger, "IP_BROKER=%s", brokerConf->ipBroker);
	brokerConf->puertoBroker = config_get_int_value(BROKERTConfig,
			"PUERTO_BROKER");
	log_info(logger, "PUERTO_BROKER=%d", brokerConf->puertoBroker);
	brokerConf->frecuenciaCompactacion = config_get_int_value(BROKERTConfig,
			"FRECUENCIA_COMPACTACION");
	log_info(logger, "FRECUENCIA_COMPACTACION=%d",
			brokerConf->frecuenciaCompactacion);
	brokerConf->logFile = string_duplicate(
			config_get_string_value(BROKERTConfig, "LOG_FILE"));
	log_info(logger, "LOG_FILE=%s", brokerConf->logFile);

	log_info(logger, "- CONFIGURACION IMPORTADA\n");

	config_destroy(BROKERTConfig);

	//ver cuando liberar el brokerConf , si lo hacemos acá no se va a poder usar en el servidor por ej,
	//estariamos cargando una estructura y liberandola sin darle uso.
	// talvez podriamos devolver la estructura y que la función no sea void.
	//hacer...
	//return brokerConf;
	//free(brokerConf);

}
//Hago esta función para acordarnos que debemos liberar el malloc de Sofi
void liberarBrokerConf() {
	free(brokerConf);
}

void inicializarColasBroker() {
	NEW_POKEMON = malloc(sizeof(t_cola));
	APPEARED_POKEMON = malloc(sizeof(t_cola));
	CATCH_POKEMON = malloc(sizeof(t_cola));
	CAUGTH_POKEMON = malloc(sizeof(t_cola));
	GET_POKEMON = malloc(sizeof(t_cola));
	LOCALIZED_POKEMON = malloc(sizeof(t_cola));

	NEW_POKEMON->cola = queue_create();
	NEW_POKEMON->lista = list_create();
	APPEARED_POKEMON->cola = queue_create();
	APPEARED_POKEMON->lista = list_create();
	CATCH_POKEMON->cola = queue_create();
	CATCH_POKEMON->lista = list_create();
	CAUGTH_POKEMON->cola = queue_create();
	CAUGTH_POKEMON->lista = list_create();
	GET_POKEMON->cola = queue_create();
	GET_POKEMON->lista = list_create();
	LOCALIZED_POKEMON->cola = queue_create();
	LOCALIZED_POKEMON->lista = list_create();

	NEW_APPEARED_POKEMON = malloc(sizeof(t_parejaCola));
	CATCH_CAUGTH_POKEMON = malloc(sizeof(t_parejaCola));
	GET_LOCALIZED_POKEMON = malloc(sizeof(t_parejaCola));
	return;
}

void destruirColasBroker() {
	queue_destroy(NEW_POKEMON->cola);
	list_destroy(NEW_POKEMON->lista);
	queue_destroy(APPEARED_POKEMON->cola);
	list_destroy(APPEARED_POKEMON->lista);
	queue_destroy(CATCH_POKEMON->cola);
	list_destroy(CATCH_POKEMON->lista);
	queue_destroy(CAUGTH_POKEMON->cola);
	list_destroy(CAUGTH_POKEMON->lista);
	queue_destroy(GET_POKEMON->cola);
	list_destroy(GET_POKEMON->lista);
	queue_destroy(LOCALIZED_POKEMON->cola);
	list_destroy(LOCALIZED_POKEMON->lista);

	free(NEW_POKEMON);
	free(APPEARED_POKEMON);
	free(CATCH_POKEMON);
	free(CAUGTH_POKEMON);
	free(CATCH_CAUGTH_POKEMON);
	free(LOCALIZED_POKEMON);

	free(NEW_APPEARED_POKEMON);
	free(CATCH_CAUGTH_POKEMON);
	free(GET_LOCALIZED_POKEMON);

}
//ver que se agrega a la lista y a la cola
void agregarMensaje(t_cola *cola, void* mensaje) {

	queue_push(cola->cola, mensaje);
	list_add(cola->lista, mensaje);

}
//llamarla en la funcion main
void pedirMemoriaInicial() {

//	void* miMemoria = malloc(brokerConf->tamanoMemoria);
//	miMemoria = 0;
}

//ver bien el argumento que le pasamos a la funcion & * o nada, ver si es tipo mensaje o char*
char* sacarMensaje(t_cola *cola) {
	char* mensaje;
	return mensaje = (char*) queue_pop(cola->cola);
}

void administrarColas(t_paquete *stream, void* clienteFd) {

	t_suscriptor *suscriptor = malloc(sizeof(t_suscriptor));
	t_mensaje *mensaje = malloc(sizeof(t_mensaje));

//	t_paquete *bufferLoco = malloc(sizeof(t_paquete));
//	bufferLoco->buffer->stream =stream;
//	bufferLoco->buffer->size=strlen(stream);
//	int opCode = bufferLoco->codigoOperacion;
//	int colaMensaje = bufferLoco->colaMensaje;

	printf(
			" Mi pid es : %d,\n mi opCode es : %d,\n mi colaMensaje es : %d.\n El puto nombre que quiero recibir es : %s y el size es : %d.\n",
			stream->pid, stream->codigoOperacion, stream->colaMensaje,
			(char*) stream->buffer->stream, (int) stream->buffer->size);

	switch (stream->codigoOperacion) {
	case SUSCRIPCION: {
		switch (stream->colaMensaje) {

		case tNEW_POKEMON: {
			list_add(NEW_POKEMON->lista, stream->buffer->stream);
			printf("meti algo en la lista : ");
			printf("%s", (char*) NEW_POKEMON->lista->head->data);
			//crearMensaje();
			//liberarConexion(clienteFd);
			//devolverMensaje(stream, clienteFd);//devolver mensaje, no se que tengo que devolver
			//pthread_exit(NULL);
			break;
		}
		case tAPPEARED_POKEMON: {
			list_add(APPEARED_POKEMON->lista, suscriptor);
			//devolverMensaje();
			break;
		}

		case tCATCH_POKEMON: {
			list_add(CATCH_POKEMON->lista, suscriptor);
			//devolverMensaje();
			break;
		}

		case tCAUGTH_POKEMON: {
			list_add(CAUGTH_POKEMON->lista, suscriptor);
			//devolverMensaje();
			break;
		}

		case tGET_POKEMON: {
			list_add(GET_POKEMON->lista, suscriptor);
			//devolverMensaje();
			break;
		}

		case tLOCALIZED_POKEMON: {
			list_add(LOCALIZED_POKEMON->lista, suscriptor);
			//devolverMensaje();
			break;
		}
		default: {
			printf("error de modulo, no se conoce quien envia paquetes\n");
		}

		}
		case MENSAJE:
		{
			switch (stream->colaMensaje) {

			case tNEW_POKEMON: {
				queue_push(NEW_POKEMON->cola, stream->buffer);
				//devolverMensaje();
				printf("meti algo en la lista : %s",
						(char*) stream->buffer->stream);

				break;
			}
			case tAPPEARED_POKEMON: {
				//list_add(APPEARED_POKEMON->lista, suscriptor);
				//devolverMensaje();
				break;
			}

			case tCATCH_POKEMON: {
				//list_add(CATCH_POKEMON->lista, suscriptor);
				//devolverMensaje();
				break;
			}

			case tCAUGTH_POKEMON: {
				//list_add(CAUGTH_POKEMON->lista, suscriptor);
				//devolverMensaje();
				break;
			}

			case tGET_POKEMON: {
				//list_add(GET_POKEMON->lista, suscriptor);
				//devolverMensaje();
				break;
			}

			case tLOCALIZED_POKEMON: {
				//list_add(LOCALIZED_POKEMON->lista, suscriptor);
				//devolverMensaje();
				break;
			}
			default: {
				printf("error de modulo, no se conoce quien envia paquetes\n");
			}
			}
		}
	}

		free(suscriptor);
		free(mensaje);
	}
	//free(bufferLoco);

}

void* handler(void* socketConectado) {
	int socket = *(int*) socketConectado;
	int size = 0;///// inicializo la variable para que llegue bien el primer mensaje
	t_paquete *bufferLoco = malloc(sizeof(t_paquete));

	bufferLoco = recibirMensaje(socket, &size);
	// devolver confirmacion al team
	//char* ack = "ack";
	//devolverMensajeConfirmacion(ack, socket);

	administrarColas(bufferLoco, socketConectado);

	log_info(logger, "Estoy dentro del handler loco\n");

	//hacer un free completo de bufferLoco
	free(bufferLoco);
	//free_t_message(bufferLoco);
	return NULL;
}
void iniciarServidor(char *ip, int puerto) {
	int socketDelCliente;
	struct sockaddr direccionCliente;
	unsigned int tamanioDireccion = sizeof(direccionCliente);
	int servidor = initServer(ip, puerto);

	log_info(logger, "ESCHUCHANDO CONEXIONES");
	log_info(logger, "iiiiIIIII!!!");
	while ((socketDelCliente = accept(servidor, (void*) &direccionCliente,
			&tamanioDireccion)) >= 0) {
		pthread_t threadId;
		log_info(logger, "Se ha aceptado una conexion: %i\n", socketDelCliente);
		if ((pthread_create(&threadId, NULL, handler, (void*) &socketDelCliente))
				< 0) {
			log_info(logger, "No se pudo crear el hilo");
			//return 1;
		} else {
			log_info(logger, "Handler asignado\n");
		}

	}
	if (socketDelCliente < 0) {
		log_info(logger, "Falló al aceptar conexión");
	}
	close(servidor);
}

