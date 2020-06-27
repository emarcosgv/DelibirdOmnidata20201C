#include "broker.h"

void inicializarLogger() {
	//Crea el logger
	logger = log_create("BROKER.log", "BROKER", 1, LOG_LEVEL_TRACE);
}

void inicializarLoggerEntregable() {
	printf("Voy a crear un logger %s\n", brokerConf->logFile);

	logEntrega = log_create(brokerConf->logFile, "BROKER", 1, LOG_LEVEL_TRACE);
	if (logEntrega == NULL) {
		perror("No se pudo inicializar el logger para la entrega\n");
	}
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
	brokerConf->algoritmoParticionLibre = string_duplicate(
			config_get_string_value(BROKERTConfig, "ALGORITMO_PARTICION_LIBRE"));
	log_info(logger, "ALGORITMO_PARTICION_LIBRE=%s", brokerConf->algoritmoParticionLibre);
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
	CAUGHT_POKEMON = malloc(sizeof(t_cola));
	GET_POKEMON = malloc(sizeof(t_cola));
	LOCALIZED_POKEMON = malloc(sizeof(t_cola));

	NEW_POKEMON->cola = list_create();
	NEW_POKEMON->lista = list_create();
	APPEARED_POKEMON->cola = list_create();
	APPEARED_POKEMON->lista = list_create();
	CATCH_POKEMON->cola = list_create();
	CATCH_POKEMON->lista = list_create();
	CAUGHT_POKEMON->cola = list_create();
	CAUGHT_POKEMON->lista = list_create();
	GET_POKEMON->cola = list_create();
	GET_POKEMON->lista = list_create();
	LOCALIZED_POKEMON->cola = list_create();
	LOCALIZED_POKEMON->lista = list_create();

	return;
}

void destruirColasBroker() {
	list_destroy(NEW_POKEMON->cola);
	list_destroy(NEW_POKEMON->lista);
	list_destroy(APPEARED_POKEMON->cola);
	list_destroy(APPEARED_POKEMON->lista);
	list_destroy(CATCH_POKEMON->cola);
	list_destroy(CATCH_POKEMON->lista);
	list_destroy(CAUGHT_POKEMON->cola);
	list_destroy(CAUGHT_POKEMON->lista);
	list_destroy(GET_POKEMON->cola);
	list_destroy(GET_POKEMON->lista);
	list_destroy(LOCALIZED_POKEMON->cola);
	list_destroy(LOCALIZED_POKEMON->lista);

	free(NEW_POKEMON);
	free(APPEARED_POKEMON);
	free(CATCH_POKEMON);
	free(CAUGHT_POKEMON);
	free(GET_POKEMON);
	free(LOCALIZED_POKEMON);

}

//llamarla en la funcion main
void pedirMemoriaInicial() {

//	miMemoria = malloc(brokerConf->tamanoMemoria);
//	offset = 0;
//	iniMemoria = &miMemoria;
//	numeroParticion = 0;
	log_info(logger, "Iniciar CACHE");

	instanteCache = 0;		// para guardar el instante en que ocurre cada movimiento de la cache.
	log_info(logger, "instante = %d", instanteCache);

	cache = malloc(brokerConf->tamanoMemoria);
	log_info(logger, "Memoria del CACHE %x-%x Largo %d  FAKE", cache,
							cache + brokerConf->tamanoMemoria-1, (brokerConf->tamanoMemoria)*sizeof(cache));
	auxTra = (int) cache;
	sizeTra = auxTra + brokerConf->tamanoMemoria-1;
	log_info(logger, "Memoria del CACHE %x-%x Largo %d  TRUE", auxTra, sizeTra, sizeTra-auxTra+1);

// Iniciamos los valores de la cache vacia.

	log_info(logger, "Size of: Nodo=%d", sizeof(struct nodoListaCache));

	particionActual = (t_nodoListaCache) malloc(sizeof(struct nodoListaCache));

	particionActual->inicio = 0;
	particionActual->fin = brokerConf->tamanoMemoria-1;
	particionActual->largo = brokerConf->tamanoMemoria;
	particionActual->estado = 0;
	particionActual->instante = instanteCache;
	particionActual->id = 0;
	particionActual->sgte = NULL;
	particionActual->ant = NULL;
	particionActual->mayor = NULL;
	particionActual->menor = NULL;

	instanteCache++; // instante de iniciado de la CACHE es siempre 0.

	particionFirst = particionActual;
	particionLast = particionActual;
	particionBig = particionActual;
	particionSmall = particionActual;

	log_info(logger, "Part.1 %x-%x  %d", particionFirst->inicio, particionFirst->fin, particionFirst->estado);
	log_info(logger, "Part.1 %x-%x  %d", particionLast->inicio, particionLast->fin, particionLast->estado);
	log_info(logger, "Part.1 %x-%x  %d", particionBig->inicio, particionBig->fin, particionBig->estado);
	log_info(logger, "Part.1 %x-%x  %d\n", particionSmall->inicio, particionSmall->fin, particionSmall->estado);

	nodos = mostrarCache( particionFirst, 0);
	nodos = mostrarCache( particionLast, 1);
	nodos = mostrarCache( particionBig, 2);
	nodos = mostrarCache( particionSmall, 3);

// pa probar
	log_info(logger, "Part.1 %x-%x  %d", particionActual->inicio, particionActual->fin, particionActual->estado);
	praLibre = encontrarLibre(4,0);
	log_info(logger, "Part.1 %x-%x  %d", praLibre->inicio, praLibre->fin, praLibre->estado);
	nodos = insertarPartition("ABCD", 4, 233, 0);
	nodos = mostrarCache( particionFirst, 0);

	praLibre = encontrarLibre(45,0);
	log_info(logger, "Part.1 %x-%x  %d", praLibre->inicio, praLibre->fin, praLibre->estado);
//	nodos = insertarPartition("123456789012345678901234567890123456789012345", 45, 234, 0);
//	nodos = mostrarCache( particionFirst, 0);

	praLibre = encontrarLibre(60,0);
	log_info(logger, "Part.1 %x-%x  %d", praLibre->inicio, praLibre->fin, praLibre->estado);
//	nodos = insertarPartition("12345678901234567890123456789012345678900123456789001234567890", 60, 235, 0);
//	nodos = mostrarCache( particionFirst, 0);


}

t_nodoListaCache encontrarLibre(int size, int orden){
	int tamanoABuscar = brokerConf->tamanoMinimoParticion; int pos = 0;
		switch(orden) {
		    case 0 : particionActual = particionFirst; break;
			case 1 : particionActual = particionLast;  break;
			case 2 : particionActual = particionBig;   break;
			case 3 : particionActual = particionSmall; break;
			default :particionActual = particionFirst;}  // sgte por defecto.
	if (size > tamanoABuscar) tamanoABuscar = size;
	log_info(logger, "Buscando  [%d] [%d] en la CACHE",size,tamanoABuscar);
	while(particionActual != NULL){ pos++;
		if((particionActual->estado == 0) && ( particionActual->largo>= tamanoABuscar)){
			log_info(logger,"Encontre [%d] de [%d]b posicion[%d]", particionActual->inicio, particionActual->largo, pos);
			return particionActual; }
		switch(orden) {
		    case 0 : particionActual = particionActual->sgte; break;
		    case 1 : particionActual = particionActual->ant;  break;
		    case 2 : particionActual = particionActual->mayor;break;
		    case 3 : particionActual = particionActual->menor;break;
		    default :particionActual = particionActual->sgte; } // sgte por defecto.
	}
	log_info(logger, "No hay particion libre donde quepa [%d]", size);
return NULL;}
int insertarPartition(void* mensaje, int size, int id, int orden){
	int tamanoABuscar = brokerConf->tamanoMinimoParticion; int pos = 0;
	t_nodoListaCache particionActual = malloc(sizeof(struct nodoListaCache)); //, prox = new nodoL;
	switch(orden) {
	    case 0 : particionActual = particionFirst; break;
		case 1 : particionActual = particionLast;  break;
		case 2 : particionActual = particionBig;   break;
		case 3 : particionActual = particionSmall; break;
		default :particionActual = particionFirst; } // sgte por defecto.
	if (size > tamanoABuscar) tamanoABuscar = size;
	log_info(logger, "Buscando  [%d] [%d] en la CACHE",size,tamanoABuscar);
	while(particionActual != NULL){	pos++;
		if((particionActual->estado == 0) && ( particionActual->largo>= tamanoABuscar)){
			log_info(logger,"Encontre [%d] de [%d]b posicion[%d]", particionActual->inicio, particionActual->largo, pos);
//     Aca hay que operar
	if (particionActual->largo == tamanoABuscar){
		particionActual->estado = size;
		particionActual->id = id;
		particionActual->instante = instanteCache;
		instanteCache++;
		nodos = mostrarCache( particionFirst, 0);
		return 0; }
	t_nodoListaCache particionNueva = (t_nodoListaCache) malloc(sizeof(struct nodoListaCache));
    particionNueva->inicio    = particionActual->inicio;
    particionNueva->fin       = particionActual->inicio+tamanoABuscar-1;
    particionNueva->largo     = tamanoABuscar;
    particionNueva->estado    = size;
    particionNueva->id        = id;
    particionNueva->instante  = instanteCache;
    particionNueva->sgte = particionActual;
    particionNueva->ant = particionActual->ant;
//  particionNueva->mayor = ???? ;
//  particionNueva->menor = ???? ;
    instanteCache++;
    particionActual->inicio   =particionActual->inicio+tamanoABuscar;
    particionActual->largo    =particionActual->largo-tamanoABuscar;
    particionActual->instante =instanteCache;
    particionActual->ant      =particionNueva;
	nodos = mostrarCache( particionFirst, 0);
	particionFirst = particionNueva;
//	particionLast = ????;
    	return 0;}
		switch(orden) {
		    case 0 : particionActual = particionActual->sgte; break;
		    case 1 : particionActual = particionActual->ant;  break;
		    case 2 : particionActual = particionActual->mayor;break;
		    case 3 : particionActual = particionActual->menor;break;
		    default :particionActual = particionActual->sgte;  // sgte por defecto.
	}}
	log_info(logger, "No hay particion libre donde quepa [%d]", size);
return 1;
}
void* buscarEspacioDisponible(int sizeMensaje) {


//	if (brokerConf->algoritmoParticionLibre=="BF"){
//		log_info(logger, "Algoritmo de particion libre Best fit NO IMPLEMENTADO");
//	}
//	if (brokerConf->algoritmoParticionLibre!="FF"){
//		log_info(logger, "Algoritmo de particion libre Desconocido");
//	}

//
//	listarCacheFirst(particionFirst);
//	t_nodoCache *aux = particionFirst;
//	int particion = 0;
//		Do {
//			printf("Dump de menor a mayor\n");
//			printf("Particion %d"), particion;
//			printf(" %h "); cache+t_nodoCache->first
//			printf("la memoria arranca en la direccion : %d .\n",int insertarPartition(void* mensaje, int size, int id, int orden) (int) iniMemoria);
//			 printf("la memoria finaliza en la direccion : %d .\n", (int) finMemoria);
//			 //	print		//			printf("Dump de menor a mayor\n");
	//			printf("Particion %d"), particion;
	//			printf(" %h "); cache+t_nodoCache->first
	//			printf("la memoria arranca en la direccion : %d .\n", (int) iniMemoria);
	//			 printf("la memoria finaliza en la direccion : %d .\n", (int) finMemoria);
	//			 //	printf("memoria total : %d .\n", memoriaTotal);
	//			 particion++;"\n\n    buscando  [%d] en la lista \n\n",n
	//
//
// f("memoria total : %d .\n", memoriaTotal);
//			 particion++;
//	}while (aux->mayor);


// int insertarPartition(void* mensaje, int size, int id, int orden)
//	return;
//

//void insertarEnCache(void* mensaje, int size, int id) {

	// Implementamos Algoritmo Particion Libre FF
	// La lista de particiones siempre tendra, al menos, un Nodo
	//

//	int tamanoABuscar = brokerConf->tamanoMinimoParticion;
///	if (tamanoABuscar<size) tamanoABuscar=size;




//	}

//}
//
int mostrarCache(t_nodoListaCache nodo, int orden) {
	int i=0;
	while(nodo!=NULL) {
		i++;
		log_info(logger,"Part:%d [%x-%x] %d<=> %d",i, nodo->inicio, nodo->fin, nodo->estado, orden);

		switch(orden) {
		    case 0 : nodo = nodo->sgte; break;
		    case 1 : nodo = nodo->ant;  break;
		    case 2 : nodo = nodo->mayor;break;
		    case 3 : nodo = nodo->menor;break;
		    default :nodo = nodo->sgte;  // sgte por defecto.
		}
	}

	return i;
}
//	memcpy(iniMemoria + offset, mensaje, size);
//	offset += size;
//	numeroParticion++;

//}
/*
 void* manejarMemoria() {

 t_paquete* paquete = malloc(sizeof(t_paquete));

 //pthread_mutex_lock(bandeja);
 //paquete = queue_peek(bandeja);

 int *iniMemoria = (int*) &miMemoria;
 int *finMemoria = iniMemoria + brokerConf->tamanoMemoria;//aritmetica de punteros para calcular el final de la memoria
 int memoriaTotal = finMemoria - iniMemoria;
 //int i = 0;
 //int estado = 0;
 int cantidadBloques = (brokerConf->tamanoMemoria
 / brokerConf->tamanoMinimoParticion) - 1;
 offSet = 0;
 int sizesTotalMensajes = 0;
 int memoriaDisponible = memoriaTotal - sizesTotalMensajes;//esto es cualquiera...
 //t_bitarray* bitMap = bitarray_create(miMemoria, cantidadBloques);

 //int tabla[][5];

 int* punteroBase = iniMemoria;
 //mem_hexdump(miMemoria, paquete->buffer->largoNombre);

 if (memoriaDisponible > paquete->buffer->largoNombre) {
 memcpy(miMemoria + offSet, paquete, paquete->buffer->largoNombre);//aca va el tamaño del mensaje y el mensaje
 offSet += paquete->buffer->largoNombre;
 punteroBase += offSet;
 //		for (i; i < paquete->buffer->largoNombre; i++) {
 //			bitarray_set_bit(bitMap, i);
 //		}			/
 }
 printf("la memoria arranca en la direccion : %d .\n", (int) iniMemoria);
 printf("la memoria finaliza en la direccion : %d .\n", (int) finMemoria);
 //	printf("memoria total : %d .\n", memoriaTotal);

 return NULL;
 }
 */
void* administrarMensajes() {

void verificarSuscriptor(t_suscriptor* suscriptor, t_list* lista) { //esto es para ver si se reconecto o si es nuevo.

	t_suscriptor* suscriptorExistente = malloc(sizeof(t_suscriptor));
	int i = 0;
	int flag = 0;
	for (i = 0; i < list_size(lista); i++) {
		suscriptorExistente = list_get(lista, i);
		if ((strcmp(suscriptor->nombreProceso,
				suscriptorExistente->nombreProceso)) == 0) { //falta ver como se guardan los ack enviados

			list_replace(lista, i, suscriptor);
			flag = 1;
			break;
		}
		if (flag == 0) {
			list_add(lista, suscriptor);
		}
	}

}

void enviarMensajeASuscriptores(t_list* lista, t_paquete* mensaje) {
	t_suscriptor* suscriptorExistente = malloc(sizeof(t_suscriptor));
	int i;
	for (i = 0; i < list_size(lista); i++) {
		suscriptorExistente = list_get(lista, i);
		switch (mensaje->codigoOperacion) {
		case MENSAJE_NEW_POKEMON: {
			enviarMensajeBrokerNew(mensaje->buffer->nombrePokemon,
					mensaje->buffer->posX, mensaje->buffer->posY,
					mensaje->buffer->cantidadPokemons,
					suscriptorExistente->socket);

			list_replace(lista, i, suscriptorExistente);
			break;
		}
		case MENSAJE_APPEARED_POKEMON: {
			enviarMensajeBrokerAppeared(mensaje->buffer->nombrePokemon,
					mensaje->buffer->posX, mensaje->buffer->posY,
					mensaje->buffer->idMensajeCorrelativo,
					suscriptorExistente->socket);
			suscriptorExistente->enviado = 1;
			list_replace(lista, i, suscriptorExistente);
			break;
		}
		case MENSAJE_GET_POKEMON: {
			enviarMensajeBrokerGet(mensaje->buffer->nombrePokemon,
					suscriptorExistente->socket);
			suscriptorExistente->enviado = 1;
			list_replace(lista, i, suscriptorExistente);
			break;
		}
		case MENSAJE_CATCH_POKEMON: {
			enviarMensajeBrokerCatch(mensaje->buffer->nombrePokemon,
					mensaje->buffer->posX, mensaje->buffer->posY,
					suscriptorExistente->socket);
			suscriptorExistente->enviado = 1;
			list_replace(lista, i, suscriptorExistente);
			break;
		}
		case MENSAJE_CAUGHT_POKEMON: {
			enviarMensajeBrokerCaught(mensaje->buffer->idMensajeCorrelativo,
					mensaje->buffer->boolean, suscriptorExistente->socket);
			suscriptorExistente->enviado = 1;
			list_replace(lista, i, suscriptorExistente);
			break;
		}
		case MENSAJE_LOCALIZED_POKEMON: {

			enviarMensajeLocalized(mensaje->buffer->nombrePokemon,
					mensaje->buffer->listaCoordenadas,
					suscriptorExistente->socket);
			break;
		}
		default: {
			printf("error de mensaje o de suscriptor.\n");
		}
		}
	}
	free(suscriptorExistente);
}

void* administrarMensajes() {

	t_paquete* paquete;

	paquete = malloc(sizeof(t_paquete));
	printf("Bloqueado en el mutex\n");
	//sem_wait(&bandejaCounter);
	pthread_mutex_lock(&bandejaMensajes_mutex);
	paquete = (t_paquete*) queue_pop(bandeja);
	pthread_mutex_unlock(&bandejaMensajes_mutex);

	printf(" Mi opCode es : %d,\n ", paquete->codigoOperacion);

	switch (paquete->codigoOperacion) {

	case SUSCRIBIRSE_NEW_POKEMON: { //falta chequear si el suscriptor existe en la lista, si existe lo dejo tal cual, sino lo agrego.
									//si existe me fijo que el socket coincida, aca es donde detecto la reconexion.
		list_add(NEW_POKEMON->lista, (void*) paquete);
		printf("meti algo en la lista : ");
		log_info(logEntrega, "Se ha suscripto a la cola New.\n");
		break;
	}
	case SUSCRIBIRSE_APPEARED_POKEMON: {
		list_add(APPEARED_POKEMON->lista, (void*) paquete);
		log_info(logEntrega, "Se ha suscripto a la cola Appeared.\n");
		break;
	}

	case SUSCRIBIRSE_CATCH_POKEMON: {
		list_add(CATCH_POKEMON->lista, (void*) paquete);
		log_info(logEntrega, "Se ha suscripto a la cola Catch.\n");
		break;
	}

	case SUSCRIBIRSE_CAUGHT_POKEMON: {
		list_add(CAUGHT_POKEMON->lista, (void*) paquete);
		log_info(logEntrega, "Se ha suscripto a la cola Caught.\n");
		break;
	}

	case SUSCRIBIRSE_GET_POKEMON: {
		list_add(GET_POKEMON->lista, (void*) paquete);
		log_info(logEntrega, "Se ha suscripto a la cola Get.\n");
		break;
	}

	case SUSCRIBIRSE_LOCALIZED_POKEMON: {
		list_add(LOCALIZED_POKEMON->lista, (void*) paquete);
		log_info(logEntrega, "Se ha suscripto a la cola Localized.\n");
		break;
	}

	case MENSAJE_NEW_POKEMON: {
		log_info(logEntrega, "Llego un mensaje nuevo a la cola New.\n");

		enviarMensajeASuscriptores(NEW_POKEMON->lista, paquete);//esto hay que probarlo.
		//envia solo el mensaje nuevo, pero hay que recorrer la cache y recuperar los mensajes también.
		t_newPokemon* bufferLoco = malloc(sizeof(t_newPokemon));
		bufferLoco->sizeNombre = paquete->buffer->largoNombre;
		bufferLoco->pokemon = paquete->buffer->nombrePokemon;
		bufferLoco->cantidadPokemons = paquete->buffer->cantidadPokemons;
		bufferLoco->posX = paquete->buffer->posX;
		bufferLoco->posY = paquete->buffer->posY;

		uint32_t sizeMensaje = sizeof(uint32_t) * 4 + bufferLoco->sizeNombre;

		int desplazamiento = 0;
		void* buffer = malloc(sizeMensaje);
		memcpy(buffer + desplazamiento, &bufferLoco->sizeNombre,
				sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		memcpy(buffer + desplazamiento, bufferLoco->pokemon,
				bufferLoco->sizeNombre);
		desplazamiento += bufferLoco->sizeNombre;

		memcpy(buffer + desplazamiento, &bufferLoco->cantidadPokemons,
				sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		memcpy(buffer + desplazamiento, &bufferLoco->posX, sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		memcpy(buffer + desplazamiento, &bufferLoco->posY, sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);

//		t_particionLibre *particion = insertarEnCache(buffer, sizeMensaje);
//
//		t_administrativo* bufferAdmin = malloc(sizeof(t_administrativo));
//		bufferAdmin->idMensaje = paquete->buffer->idMensaje;
//		bufferAdmin->colaMensaje = MENSAJE_NEW_POKEMON;
//		bufferAdmin->suscriptoresEnviados = list_create();
//		bufferAdmin->suscriptoresRecibidos = list_create();
//		bufferAdmin->particion = particion->particion;
//		bufferAdmin->offsetInicioParticion = particion->offsetInicioParticion;
//		bufferAdmin->sizeParticion = particion->sizeParticion;
//		bufferAdmin->sizeMensajeGuardado = sizeMensaje;
//		bufferAdmin->flagLRU = particion->flagLRU;

		//list_add(NEW_POKEMON->cola, bufferAdmin);
		printf(" ENCOLE EN NEW : %s . \n", bufferLoco->pokemon);
		break;
	}
	case MENSAJE_APPEARED_POKEMON: {
		log_info(logEntrega, "Llego un mensaje nuevo a la cola Appeared.\n");
		t_appearedPokemon* bufferLoco = malloc(sizeof(t_appearedPokemon));
		bufferLoco->sizeNombre = paquete->buffer->largoNombre;
		bufferLoco->pokemon = paquete->buffer->nombrePokemon;
		bufferLoco->posX = paquete->buffer->posX;
		bufferLoco->posY = paquete->buffer->posY;

		uint32_t sizeMensaje = sizeof(uint32_t) * 3 + bufferLoco->sizeNombre;

		int desplazamiento = 0;
		void* buffer = malloc(sizeMensaje);
		memcpy(buffer + desplazamiento, &bufferLoco->sizeNombre,
				sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		memcpy(buffer + desplazamiento, bufferLoco->pokemon,
				bufferLoco->sizeNombre);
		desplazamiento += bufferLoco->sizeNombre;

		memcpy(buffer + desplazamiento, &bufferLoco->posX, sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		memcpy(buffer + desplazamiento, &bufferLoco->posY, sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		///aca va un semaforo para insertar en la cache
//		t_particionLibre *particion = insertarEnCache(buffer, sizeMensaje);
//
//		t_administrativo* bufferAdmin = malloc(sizeof(t_administrativo));
//		bufferAdmin->idMensaje = paquete->buffer->idMensaje;
//		bufferAdmin->colaMensaje = MENSAJE_APPEARED_POKEMON;
//		bufferAdmin->suscriptoresEnviados = list_create();
//		bufferAdmin->suscriptoresRecibidos = list_create();
//		bufferAdmin->particion = particion->particion;
//		bufferAdmin->offsetInicioParticion = particion->offsetInicioParticion;
//		bufferAdmin->sizeParticion = particion->sizeParticion;
//		bufferAdmin->sizeMensajeGuardado = sizeMensaje;
//		bufferAdmin->flagLRU = particion->flagLRU;

		//list_add(APPEARED_POKEMON->cola, bufferAdmin);
		printf("ENCOLE EN APPEARED : %s . \n", bufferLoco->pokemon);
		break;
	}

	case MENSAJE_CATCH_POKEMON: {
		log_info(logEntrega, "Llego un mensaje nuevo a la cola Catch.\n");
		t_catchPokemon* bufferLoco = malloc(sizeof(t_catchPokemon));
		bufferLoco->sizeNombre = paquete->buffer->largoNombre;
		bufferLoco->pokemon = paquete->buffer->nombrePokemon;
		bufferLoco->posX = paquete->buffer->posX;
		bufferLoco->posY = paquete->buffer->posY;

		uint32_t sizeMensaje = sizeof(uint32_t) * 3 + bufferLoco->sizeNombre;

		int desplazamiento = 0;
		void* buffer = malloc(sizeMensaje);
		memcpy(buffer + desplazamiento, &bufferLoco->sizeNombre,
				sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		memcpy(buffer + desplazamiento, bufferLoco->pokemon,
				bufferLoco->sizeNombre);
		desplazamiento += bufferLoco->sizeNombre;

		memcpy(buffer + desplazamiento, &bufferLoco->posX, sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		memcpy(buffer + desplazamiento, &bufferLoco->posY, sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);

		///aca va un semaforo para insertar en la cache
//		t_particionLibre *particion = insertarEnCache(buffer, sizeMensaje);
//
//		t_administrativo* bufferAdmin = malloc(sizeof(t_administrativo));
//		bufferAdmin->idMensaje = paquete->buffer->idMensaje;
//		bufferAdmin->colaMensaje = MENSAJE_CATCH_POKEMON;
//		bufferAdmin->suscriptoresEnviados = list_create();
//		bufferAdmin->suscriptoresRecibidos = list_create();
//		bufferAdmin->particion = particion->particion;
//		bufferAdmin->offsetInicioParticion = particion->offsetInicioParticion;
//		bufferAdmin->sizeParticion = particion->sizeParticion;
//		bufferAdmin->sizeMensajeGuardado = sizeMensaje;
//		bufferAdmin->flagLRU = particion->flagLRU;

		//list_add(CATCH_POKEMON->cola, bufferAdmin);

		printf("ENCOLE EN CATCH : %s . \n", bufferLoco->pokemon);
		break;
	}

	case MENSAJE_CAUGHT_POKEMON: {
		log_info(logEntrega, "Llego un mensaje nuevo a la cola Caught.\n");
		t_caughtPokemon* bufferLoco = malloc(sizeof(t_caughtPokemon));
		bufferLoco->booleano = paquete->buffer->boolean;

		uint32_t sizeMensaje = sizeof(uint32_t);
		int desplazamiento = 0;
		void* buffer = malloc(sizeMensaje);
		memcpy(buffer + desplazamiento, &bufferLoco->booleano, sizeof(int));
		desplazamiento += sizeof(int);

//		t_particionLibre *particion = insertarEnCache(buffer, sizeMensaje);
//
//		t_administrativo* bufferAdmin = malloc(sizeof(t_administrativo));
//		bufferAdmin->idMensaje = paquete->buffer->idMensaje;
//		bufferAdmin->colaMensaje = MENSAJE_CAUGHT_POKEMON;
//		bufferAdmin->suscriptoresEnviados = list_create();
//		bufferAdmin->suscriptoresRecibidos = list_create();
//		bufferAdmin->particion = particion->particion;
//		bufferAdmin->offsetInicioParticion = particion->offsetInicioParticion;
//		bufferAdmin->sizeParticion = particion->sizeParticion;
//		bufferAdmin->sizeMensajeGuardado = sizeMensaje;
//		bufferAdmin->flagLRU = particion->flagLRU;
		//list_add(CAUGHT_POKEMON->cola, bufferAdmin);
		printf("ENCOLE EN CAUGHT : %d . \n", bufferLoco->booleano);
		break;
	}

	case MENSAJE_GET_POKEMON: {
		log_info(logEntrega, "Llego un mensaje nuevo a la cola Get.\n");
		t_getPokemon* bufferLoco = malloc(sizeof(t_catchPokemon));
		bufferLoco->sizeNombre = paquete->buffer->largoNombre;
		bufferLoco->pokemon = paquete->buffer->nombrePokemon;

		uint32_t sizeMensaje = sizeof(uint32_t) + bufferLoco->sizeNombre;

		int desplazamiento = 0;
		void* buffer = malloc(sizeMensaje);
		memcpy(buffer + desplazamiento, &bufferLoco->sizeNombre,
				sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		memcpy(buffer + desplazamiento, bufferLoco->pokemon,
				bufferLoco->sizeNombre);
		desplazamiento += bufferLoco->sizeNombre;

//		t_particionLibre *particion = insertarEnCache(buffer, sizeMensaje);
//
//		t_administrativo* bufferAdmin = malloc(sizeof(t_administrativo));
//		bufferAdmin->idMensaje = paquete->buffer->idMensaje;
//		bufferAdmin->colaMensaje = MENSAJE_GET_POKEMON;
//		bufferAdmin->suscriptoresEnviados = list_create();
//		bufferAdmin->suscriptoresRecibidos = list_create();
//		bufferAdmin->particion = particion->particion;
//		bufferAdmin->offsetInicioParticion = particion->offsetInicioParticion;
//		bufferAdmin->sizeParticion = particion->sizeParticion;
//		bufferAdmin->sizeMensajeGuardado = sizeMensaje;
//		bufferAdmin->flagLRU = particion->flagLRU;
		//list_add(GET_POKEMON->cola, bufferAdmin);
		printf("ENCOLE EN GET : %s . \n", bufferLoco->pokemon);

		break;
	}

	case MENSAJE_LOCALIZED_POKEMON: {
		log_info(logEntrega, "Llego un mensaje nuevo a la cola Localized.\n");
		t_localizedPokemon* bufferLoco = malloc(sizeof(t_localizedPokemon));

		bufferLoco->sizeNombre = paquete->buffer->largoNombre;
		bufferLoco->pokemon = paquete->buffer->nombrePokemon;
		bufferLoco->cantidadDePosiciones =
				paquete->buffer->listaCoordenadas->elements_count;
		bufferLoco->posiciones = list_create();
		bufferLoco->posiciones = list_duplicate(
				paquete->buffer->listaCoordenadas);

		uint32_t sizeMensaje = sizeof(uint32_t)
				* (2 + paquete->buffer->listaCoordenadas->elements_count)
				+ bufferLoco->sizeNombre;

		uint32_t desplazamiento = 0;
		void* buffer = malloc(sizeMensaje);
		memcpy(buffer + desplazamiento, &bufferLoco->sizeNombre,
				sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		memcpy(buffer + desplazamiento, bufferLoco->pokemon,
				bufferLoco->sizeNombre);
		desplazamiento += bufferLoco->sizeNombre;

		uint32_t cantidadCoordenadas =
				paquete->buffer->listaCoordenadas->elements_count;
		printf("Al serializar, cantidadCoordenadas=%d\n", cantidadCoordenadas);
		printf("Serializando CantidadCoordenadas=%d\n", cantidadCoordenadas);
		memcpy(buffer + desplazamiento, &cantidadCoordenadas, sizeof(int));
		desplazamiento += sizeof(uint32_t);

		t_list*aux = list_duplicate(paquete->buffer->listaCoordenadas);
		//if(cantidadCoordenadas!=0){
		while (aux->head != NULL) {
			t_posicion *buffercito;
			buffercito = (t_posicion*) aux->head->data;
			printf("Serializando coordenada %d,%d\n", buffercito->x,
					buffercito->y);
			memcpy(buffer + desplazamiento, buffercito, sizeof(t_posicion));
			desplazamiento += sizeof(t_posicion);
			aux->head = aux->head->next;
			free(buffercito);
		}

//		t_particionLibre *particion = insertarEnCache(buffer, sizeMensaje);
//
//		t_administrativo* bufferAdmin = malloc(sizeof(t_administrativo));
//		bufferAdmin->idMensaje = paquete->buffer->idMensaje;
//		bufferAdmin->colaMensaje = MENSAJE_LOCALIZED_POKEMON;
//		bufferAdmin->suscriptoresEnviados = list_create();
//		bufferAdmin->suscriptoresRecibidos = list_create();
//		bufferAdmin->particion = particion->particion;
//		bufferAdmin->offsetInicioParticion = particion->offsetInicioParticion;
//		bufferAdmin->sizeParticion = particion->sizeParticion;
//		bufferAdmin->sizeMensajeGuardado = sizeMensaje;
//		bufferAdmin->flagLRU = particion->flagLRU;
		//list_add(LOCALIZED_POKEMON->cola, bufferAdmin);
		printf("ENCOLE EN LOCALIZED : %s . \n", bufferLoco->pokemon);
		break;
	}
	case CONFIRMACION_ACK: {//tengo que actualizar los ack de los mensajes
		break;
	}
	default: {
		printf("error de modulo, no se conoce quien envia paquetes\n");
	}
	}

	//free(paquete->buffer);
	//free(paquete);

//free(paquete);
//
//	free( bufferLoco);

	printf("estoy en el final de administrar mensajes\n");
	pthread_exit(NULL);
	return NULL;
}

void* handler(void* socketConectado) {
	int socket = *(int*) socketConectado;
	pthread_mutex_t mutexRecibir;//este semaforo no lo entiendo muy bien, pero funciona, sin él se rompe todo.
	pthread_mutex_init(&mutexRecibir, NULL);
//printf("Mi semaforo vale %d\n", mutexRecibir.__data.__count);

	t_paquete *bufferLoco;
	bufferLoco = malloc(sizeof(t_paquete));
	int flag = 1;
	printf("Esperando por un nuevo mensaje...\n");

	while (flag) {

		//pthread_mutex_lock(&recibir_mutex);

		pthread_mutex_lock(&mutexRecibir);
		bufferLoco = recibirMensaje(socket);

		if (bufferLoco != NULL) {

			if (bufferLoco->codigoOperacion >= 7
					&& bufferLoco->codigoOperacion <= 12) {	//esto es para capturar suscriptores.
				printf(" Soy suscriptor a la cola %d.\n",
						bufferLoco->codigoOperacion);
				t_suscriptor* suscriptor = malloc(sizeof(t_suscriptor));
				suscriptor->codigoOperacion = bufferLoco->codigoOperacion;
				suscriptor->largoNombreProceso =
						bufferLoco->buffer->largoNombreProceso;
				suscriptor->nombreProceso = bufferLoco->buffer->nombreProceso;
				suscriptor->socket = socket;
				suscriptor->ack = 0;
				suscriptor->enviado = 0;

				//pthread_mutex_lock(&bandejaSuscriptores_mutex);
				queue_push(bandeja, (void*) suscriptor);
				sem_post(&bandejaCounter);	//esto lo copié de Brian
				//pthread_mutex_unlock(&bandejaSuscriptores_mutex);//ACA HAY QUE HACER UN SEM COMO EL DE BRIAN
				pthread_mutex_unlock(&mutexRecibir);
			} else {
				printf(" Soy un mensaje .\n");
				printf("recibi un mensaje con el nombre : %s .\n",
						bufferLoco->buffer->nombrePokemon);
				//pthread_mutex_lock(&bandejaMensajes_mutex);
				pthread_mutex_lock(&asignarIdMensaje_mutex);
				bufferLoco->buffer->idMensaje = idMensajeUnico;
				idMensajeUnico++;
				pthread_mutex_unlock(&asignarIdMensaje_mutex);

				queue_push(bandeja, (void*) bufferLoco);
				sem_post(&bandejaCounter);
				//pthread_mutex_unlock(&bandejaMensajes_mutex);
				enviarIdMensaje(idMensajeUnico, socket);

				pthread_mutex_unlock(&mutexRecibir);
				printf("estoy despues del unlock de bandeja de mensajes\n");
			}
		} else {
			pthread_mutex_unlock(&mutexRecibir);
			flag = 0;
			printf(" me desconecte .\n");
			liberarConexion(socket);
		}

		printf("Esperando por un nuevo mensaje...\n");

	}

	free(bufferLoco);

//free_t_message(bufferLoco);
	printf(" Estoy finalizando el hilo...\n");
	pthread_exit(NULL);
	return NULL;
}
void* consumirMensajes() {

	while (1) {
		pthread_t hilito;
		sem_wait(&bandejaCounter);
		//pthread_mutex_lock(&bandejaMensajes_mutex);
		pthread_create(&hilito, NULL, administrarMensajes, NULL);
		//pthread_mutex_unlock(&bandejaMensajes_mutex);

	}

	return NULL;
}
void* escucharConexiones() {
	pthread_t threadId[MAX_CONEXIONES];

	int contadorConexiones = 0;
	int socketDelCliente[MAX_CONEXIONES];
	struct sockaddr direccionCliente;
	unsigned int tamanioDireccion = sizeof(direccionCliente);

	int servidor = initServer(brokerConf->ipBroker, brokerConf->puertoBroker);

	log_info(logger, "ESCHUCHANDO CONEXIONES");
	log_info(logger, "iiiiIIIII!!!");

	while (1) {

		socketDelCliente[contadorConexiones] = accept(servidor,
				(void*) &direccionCliente, &tamanioDireccion);

		if (socketDelCliente >= 0) {

			log_info(logEntrega, "Se ha aceptado una conexion: %i\n",
					socketDelCliente[contadorConexiones]);
			if ((pthread_create(&threadId[contadorConexiones], NULL, handler,
					(void*) &socketDelCliente[contadorConexiones])) < 0) {
				log_info(logger, "No se pudo crear el hilo");
				//return 1;
			} else {
				log_info(logger, "Handler asignado\n");
				tamanioDireccion = 0;

			}
		} else {
			log_info(logger, "Falló al aceptar conexión");
		}

		contadorConexiones++;

	}
	return NULL;
}
void inicializarSemaforos() {
	pthread_mutex_init(&bandejaSuscriptores_mutex, NULL);
	pthread_mutex_init(&bandejaMensajes_mutex, NULL);
	pthread_mutex_init(&recibir_mutex, NULL);
	pthread_mutex_init(&asignarIdMensaje_mutex, NULL);
	sem_init(&bandejaCounter, 1, 0);
	sem_init(&bandejaSuscriptorCounter, 1, 0);
}
