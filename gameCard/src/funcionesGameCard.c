#include "gameCard.h"

void inicializar_logger()
{
	logger = log_create("GAMECARD.log", "GAMECARD", 1, LOG_LEVEL_TRACE);
	if(logger == NULL){
		perror("No se puso inicializar el logger\n");
		exit(1);
	}
}

void cargarConfigGameCard()
{
	gameCardConfig = (t_GAMECARDConfig*) malloc(sizeof(t_GAMECARDConfig));

	GAMECARDTConfig = config_create(GAMECARD_CONFIG_PATH);
	if(GAMECARDTConfig == NULL){
		perror("Error al abrir el archivo de configuracion");
		log_error(logger, "- NO SE PUDO IMPORTAR LA CONFIGURACION");
		exit(2);
	}

	gameCardConfig->tiempoReintentoConexion=config_get_int_value(GAMECARDTConfig,"TIEMPO_DE_REINTENTO_CONEXION");
	gameCardConfig->tiempoReintentoOperacion=config_get_int_value(GAMECARDTConfig,"TIEMPO_DE_REINTENTO_OPERACION");
	gameCardConfig->ipBroker=string_duplicate(config_get_string_value(GAMECARDTConfig,"IP_BROKER"));
	gameCardConfig->puertoBroker=config_get_int_value(GAMECARDTConfig,"PUERTO_BROKER");
	gameCardConfig->puntoDeMontaje=string_duplicate(config_get_string_value(GAMECARDTConfig,"PUNTO_MONTAJE_TALLGRASS"));
	gameCardConfig->ipGameCard=string_duplicate(config_get_string_value(GAMECARDTConfig,"IP_GAMECARD"));
	gameCardConfig->puertoGameCard=config_get_int_value(GAMECARDTConfig,"PUERTO_GAMECARD");


	log_info(logger,"- tiempoReintentoConexion=%d\n",gameCardConfig->tiempoReintentoConexion);
	log_info(logger,"- tiempoReintentoOperacion=%d\n",gameCardConfig->tiempoReintentoOperacion);
	log_info(logger,"- puertoBroker=%d\n",gameCardConfig->puertoBroker);
	log_info(logger,"- ipBroker=%s\n",gameCardConfig->ipBroker);
	log_info(logger,"- puntoDeMontaje=%s\n",gameCardConfig->puntoDeMontaje);
	log_info(logger,"- ipGameCard=%s\n",gameCardConfig->ipGameCard);
	log_info(logger,"- puertoGameCard\n",gameCardConfig->puertoGameCard);

	log_info(logger, "- CONFIG IMPORTADA CON EXITO\n");
	return;
}

void suscribirmeAColasBroker()
{
	/*
	 * 2. Suscribirse a las colas del Broker
	 * 	a. Suscribirse a tNEW_POKEMON
	 * 	b. Suscribirse a tCATCH_POKEMON
	 * 	c. Suscribirse a tGET_POKEMON
	 *
	 * 3. Recibir confirmación
	 */

	log_info(logger, "SUSCRIPCION ACEPTADA - tNEW_POKEMON\n");
	log_info(logger, "SUSCRIPCION ACEPTADA - tCATCH_POKEMON\n");
	log_info(logger, "SUSCRIPCION ACEPTADA - tGET_POKEMON\n");
	return;
}

char* crearRutaArchivo(char* nombreArchivo)
{
	char* rutaArchivo = string_new();
	string_append(&rutaArchivo, gameCardConfig->puntoDeMontaje);
	string_append(&rutaArchivo, nombreArchivo);
	log_debug(logger, "Ruta creada: %s", rutaArchivo);
	return rutaArchivo;
}

char* crearRutaPokemon(char* nombrePokemon)
{
	char* rutaArchivo = string_new();
	string_append(&rutaArchivo, gameCardConfig->puntoDeMontaje);
	string_append(&rutaArchivo, "/Files/Pokemon/");
	string_append(&rutaArchivo, nombrePokemon);
	string_append(&rutaArchivo, "/Metadata.bin");
	return rutaArchivo;
}

void crearArchivo(char* rutaArchivo, char* stringAEscribir)
{
	FILE *fp = txt_open_for_append(rutaArchivo);
	int filedescr = fileno(fp);
	flock(filedescr, LOCK_EX);
	if(fp == NULL)
	{
		log_error(logger, "Error al crear archivo %s\n", rutaArchivo);
		exit(1);
	}
	txt_write_in_file(fp, stringAEscribir);
	txt_close_file(fp);
	flock(filedescr, LOCK_UN);

	log_info(logger, "ARCHIVO %s ACTUALIZADO\n", rutaArchivo);
	return;
}
void EscribirArchivo(char* rutaArchivo, char* stringAEscribir)
{
	FILE *fp = txt_open_for_append(rutaArchivo);
	int filedescr = fileno(fp);
	flock(filedescr, LOCK_EX);
	if(fp == NULL)
	{
		log_error(logger, "Error al crear archivo %s\n", rutaArchivo);
		exit(1);
	}
	txt_write_in_file(fp, stringAEscribir);
	txt_close_file(fp);
	flock(filedescr, LOCK_UN);

	log_info(logger, "ARCHIVO %s ACTUALIZADO\n", rutaArchivo);
	return;
}

void terminarProgramaGameCard()
{
	log_destroy(logger);
	config_destroy(GAMECARDTConfig);
	free(gameCardConfig);
}

void crearCarpeta(char* ruta)
{
	struct stat st = {0};
	if (stat(ruta, &st) == -1)
	{
		//No tengo idea si los permisos 0777 son seguros

		pthread_mutex_lock(&mutex_crear_carpeta);
		int exito = mkdir(ruta, 0777);
		pthread_mutex_unlock(&mutex_crear_carpeta);
		if(exito==-1){
			log_error(logger, "No se pudo crear la carpeta: %s", ruta);
		}
	}
}

void iniciarTallGrass()
{
	crearCarpeta("/home/utnso/desktop/tall-grass");
	crearCarpeta("/home/utnso/desktop/tall-grass/Metadata");
	crearCarpeta("/home/utnso/desktop/tall-grass/Blocks");
	crearCarpeta("/home/utnso/desktop/tall-grass/Files");
	crearCarpeta("/home/utnso/desktop/tall-grass/Files/Pokemon");


	 // Crear archivos Metadata general
	char* rutaMetadata = crearRutaArchivo(RUTA_METADATA_GENERAL);
	char* rutaBitmap = crearRutaArchivo(RUTA_BITMAP_GENERAL);


	char* linea_metadata = string_new();
	string_append(&linea_metadata, "BLOCK_SIZE=64\n");
	string_append(&linea_metadata, "BLOCKS=5192\n");
	string_append(&linea_metadata, "MAGIC_NUMBER=TALL_GRASS\n");

	EscribirArchivo(rutaMetadata, linea_metadata);
	EscribirArchivo(rutaBitmap, "\n");
	return;
}

int archivoAbierto(char* rutaArchivo)
{
	/*
	 *  true   1
	 *  false  0
	 */
	return 0;
}

int existePokemon(char* rutaPokemon)
{
	/*
	 *  true   1
	 *  false  0
	 */
	if( access( rutaPokemon, F_OK ) != -1 ) {
	    // Existe el pokemon
		return 1;
	} else {
	    // No existe el pokemon
		return 0;
	}
}

char* crearBlock(int block, int x, int y, int cant)
{
	char* rutaBlocks = string_new();
	string_append(&rutaBlocks,"/Blocks/");
	string_append(&rutaBlocks,string_itoa(block));
	string_append(&rutaBlocks,".bin");
	char* ruta=crearRutaArchivo(rutaBlocks);

	char* str_block = string_new();
	string_append(&str_block,"X=");
	string_append(&str_block,string_itoa(x));
	string_append(&str_block,"\n");

	string_append(&str_block,"Y=");
	string_append(&str_block,string_itoa(y));
	string_append(&str_block,"\n");

	string_append(&str_block,"CANTIDAD=");
	string_append(&str_block,string_itoa(cant));
	string_append(&str_block,"\n");

	EscribirArchivo(ruta, str_block);
	return ruta;
}
void agregarNewPokemon(char* pokemon, int x, int y, int cantidad)
{
	char* rutaPokemon=crearRutaPokemon(pokemon);

	char* carpetaPokemon=string_new();
	string_append(&carpetaPokemon, gameCardConfig->puntoDeMontaje);
	string_append(&carpetaPokemon, "/Files/Pokemon/");
	string_append(&carpetaPokemon, pokemon);

	if(existePokemon(rutaPokemon) == 0) //NO EXISTE EL POKEMON
	{

		crearCarpeta(carpetaPokemon);

		pthread_mutex_lock(&mutex_cant_blockers);
		maximo_block_creado++;
		pthread_mutex_unlock(&mutex_cant_blockers);

		char* rutaBlock=crearBlock(maximo_block_creado,x,y,cantidad);

		// Obtener size del archivo block
		struct stat st;
		stat(rutaBlock, &st);
		int size = st.st_size;

		char* linea1Metadata=string_new();
		string_append(&linea1Metadata,"DIRECTORY=N\n");
		string_append(&linea1Metadata,"SIZE=");
		string_append(&linea1Metadata, string_itoa(size));
		string_append(&linea1Metadata,"\n");
		string_append(&linea1Metadata,"BLOCKS=[");
		string_append(&linea1Metadata, string_itoa(maximo_block_creado));
		string_append(&linea1Metadata,"]");
		string_append(&linea1Metadata,"\n");
		string_append(&linea1Metadata,"OPEN=Y");
		string_append(&linea1Metadata,"\n");

		crearArchivo(rutaPokemon,linea1Metadata);

	}else{ // EXISTE EL POKEMON

		log_info(logger, "El pokemon existe en %s", rutaPokemon);
		FILE *fp =fp = fopen(rutaPokemon, "r");
		char buff[255];

		fscanf(fp, "%s", buff);
		printf("1: %s\n", buff);

		fscanf(fp, "%s", buff);
		printf("2: %s\n", buff);
		char* ln_size_actual=string_duplicate(buff);
		char** size_array=string_split(ln_size_actual, "=");
		int int_size_actual;
		string_trim(size_array);
		sscanf(size_array[1], "%d",&int_size_actual);

		fscanf(fp, "%s", buff);
		printf("3: %s\n", buff);

		fclose(fp);
		char** block_array=string_split(buff, "=");
		string_trim(block_array);
		//string_is_empty();

		// restamos los []
		int cant_numeros=string_length(block_array[1])-2;

		char** array_strings=string_get_string_as_array(block_array[1]);

		for(int i=0; i<cant_numeros; i++)
		{
			int mismaposicion=0;
			char* rutaBlocks = string_new();
			string_append(&rutaBlocks,"/Blocks/");
			string_append(&rutaBlocks,array_strings[i]);
			string_append(&rutaBlocks,".bin");
			char* ruta=crearRutaArchivo(rutaBlocks);

			char buff2[255];

			printf("Ruta %s\n", ruta);
			fp = fopen(ruta, "r");
			fscanf(fp, "%s", buff2);
			printf("1: %s\n", buff2);

			char* s_x=string_duplicate(buff2);
			char** block_arrayx=string_split(s_x, "=");
			string_trim(block_arrayx);

			int int_x;
			sscanf(block_arrayx[1], "%d",&int_x);
			if(x==int_x){
				mismaposicion++;
			}

			fscanf(fp, "%s", buff2);
			printf("2: %s\n", buff2);

			char* s_y=string_duplicate(buff2);
			char** block_arrayy=string_split(s_y, "=");
			int int_y;
			string_trim(block_arrayy);
			sscanf(block_arrayy[1], "%d",&int_y);

			if(int_y==y){
				mismaposicion++;
			};
			fscanf(fp, "%s", buff2);
			printf("3: %s\n", buff2);
			fscanf(fp, "%s", buff2);

			char* s_cant=string_duplicate(buff2);
			char** block_arraycant=string_split(s_cant, "=");
			int int_cant;
			string_trim(block_arrayy);
			sscanf(block_arraycant[1], "%d",&int_cant);
			int cantidad_actualizada=cantidad+int_cant;

			fclose(fp);
			if(mismaposicion==2)
			{
				char temp[]="/home/utnso/desktop/tall-grass/Blocks/temp.txt";
				char* newln = string_new();
				string_append(&newln,"CANTIDAD=");
				string_append(&newln,string_itoa(cantidad_actualizada));
				string_append(&newln,"\n");

				int MAX=256;
				char str[MAX];
				FILE *fptr1, *fptr2;
				int lno, linectr =0;
				printf("Estoy por leer el archivo: %s\n",ruta);
				printf("Estoy por escribir el archivo: %s\n",temp);
				fptr1 = fopen(ruta, "r");
				fptr2 = fopen(temp, "w+");
				lno = 3;
				while (!feof(fptr1))
				{
					strcpy(str, "\0");
					fgets(str,MAX,fptr1);
					if(!feof(fptr1))
					{
						linectr++;
						printf("Linea: %d - %d", linectr, lno);
						if(linectr !=lno)
						{
							fprintf(fptr2, "%s", str);
						}else{
							fprintf(fptr2, "%s", newln);
						}
					}
				}
				fclose(fptr1);
				fclose(fptr2);
				int funciono= remove(ruta);

				printf("Funciono: %d\n", funciono);

				int funciono2=rename(temp, ruta);

				printf("Funciono el rename: %d\n", funciono2);
				printf("Remplazo realizado!!'\n");
				return;
			}
		}

		//crear nuevo block para el mismo pokemon
		pthread_mutex_lock(&mutex_cant_blockers);
		maximo_block_creado++;
		pthread_mutex_unlock(&mutex_cant_blockers);
		char* rutaBlockNuevo=crearBlock(maximo_block_creado,x,y,cantidad);
		printf("Block %s creado para el pokemon %s.\n", rutaBlockNuevo, pokemon);

		char* dir_pokemon=string_duplicate(carpetaPokemon);
		string_append(&dir_pokemon, "temp.txt");

		char* newln2 = string_new();
		string_append(&newln2,"BLOCKS=[");
		for(int i=0; i<cant_numeros; i++)
		{
			printf("Posicion: %d.\n", i);
			printf("Array: %s\n", array_strings[i]);
			printf("Block nro: %s\n", array_strings[i]);
			string_append(&newln2, array_strings[i]);
			string_append(&newln2,",");
		}

		string_append(&newln2, string_itoa(maximo_block_creado));
		string_append(&newln2,"]");
		string_append(&newln2,"\n");

		// Calcurlar nuevo size
		struct stat st2;
		stat(rutaBlockNuevo, &st2);
		int size_block_nw = st2.st_size;
		int size_actualizadp=size_block_nw + int_size_actual;
		char* linea_size=string_new();
		string_append(&linea_size,"SIZE=");
		string_append(&linea_size, string_itoa(size_actualizadp));
		string_append(&linea_size,"\n");

		int MAX=256;
		char str[MAX];
		FILE *fptr1, *fptr2;
		fptr1 = fopen(rutaPokemon, "r");
		fptr2 = fopen(dir_pokemon, "w");
		int lno_blocks=3;
		int lno_size_blocks=2;
		int linectr=0;
		printf("Linea a ser escrita en la posicion %d: %s\n",lno_size_blocks,linea_size);
		printf("Linea a ser escrita en la posicion %d: %s\n",lno_blocks,newln2);

		while (!feof(fptr1))
		{
			strcpy(str, "\0");
			fgets(str,MAX,fptr1);
			if(!feof(fptr1))
			{
				linectr++;
				if(linectr == lno_size_blocks)
				{
					fprintf(fptr2, "%s", linea_size);
				}else if(linectr == lno_blocks)
				{
					fprintf(fptr2, "%s", newln2);

				}else{
					fprintf(fptr2, "%s", str);
				}
			}
		}
		fclose(fptr1);
		fclose(fptr2);
		int funciono= remove(rutaPokemon);

		printf("Funciono: %d\n", funciono);

		int funciono2=rename(dir_pokemon, rutaPokemon);

		printf("Funciono el rename: %d\n", funciono2);
		printf("Remplazo realizado!!'\n");

		return;

	}

	return;
}

int catchPokemon(char* pokemon, int x, int y)
{
	log_info(logger,"CATCH Pokemon %s", pokemon);

	char* carpetaPokemon=string_new();
	string_append(&carpetaPokemon,"/home/utnso/desktop/tall-grass/Files/Pokemon/");
	string_append(&carpetaPokemon, pokemon);
	char* rutaPokemon=crearRutaPokemon(pokemon);

	if(existePokemon(rutaPokemon) == 0)
	{
		perror("El pokemon no existe");
		log_error(logger, "- El pokemon %s no existe", pokemon);
		return -1;
	}else{
		// 1. Verificar si las posiciones existen

		FILE *fp;
		char buff[255];

		fp = fopen(rutaPokemon, "r");
		// Linea 1
		fscanf(fp, "%s", buff);
		// Linea 2
		fscanf(fp, "%s", buff);
		char* ln_size_actual=string_duplicate(buff);
		char** size_array=string_split(ln_size_actual, "=");
		int int_size_actual;
		string_trim(size_array);
		sscanf(size_array[1], "%d",&int_size_actual);
		// Linea 3
		fscanf(fp, "%s", buff);
		fclose(fp);
		char** block_array=string_split(buff, "=");
		string_trim(block_array);

		// restamos los []
		int cant_numeros=string_length(block_array[1])-2;

		char** array_strings=string_get_string_as_array(block_array[1]);

		for(int i=0; i<cant_numeros; i++)
		{
			int mismaposicion=0;
			char* rutaBlocks = string_new();
			string_append(&rutaBlocks,"/Blocks/");
			string_append(&rutaBlocks,array_strings[i]);
			string_append(&rutaBlocks,".bin");
			char* rutaBlock=crearRutaArchivo(rutaBlocks);

			char buff2[255];
			fp = fopen(rutaBlock, "r");
			fscanf(fp, "%s", buff2);

			char* s_x=string_duplicate(buff2);
			char** block_arrayx=string_split(s_x, "=");
			string_trim(block_arrayx);

			int int_x;
			sscanf(block_arrayx[1], "%d",&int_x);
			if(x==int_x){
				mismaposicion++;
			}

			fscanf(fp, "%s", buff2);
			char* s_y=string_duplicate(buff2);
			char** block_arrayy=string_split(s_y, "=");
			int int_y;
			string_trim(block_arrayy);
			sscanf(block_arrayy[1], "%d",&int_y);

			if(int_y==y){
				mismaposicion++;
			};
			fscanf(fp, "%s", buff2);
			fscanf(fp, "%s", buff2);

			char* s_cant=string_duplicate(buff2);
			char** block_arraycant=string_split(s_cant, "=");
			int int_cant;
			string_trim(block_arrayy);
			sscanf(block_arraycant[1], "%d",&int_cant);
			if(int_cant==0)
			{
				return -1;
			}
			fclose(fp);
			if(mismaposicion==2)
			{
				char temp[]="/home/utnso/desktop/tall-grass/Blocks/temp.txt";

				int cantidad_actualizada=int_cant-1;
				char* newln = string_new();
				string_append(&newln,"CANTIDAD=");
				string_append(&newln,string_itoa(cantidad_actualizada));
				string_append(&newln,"\n");

				int MAX=256;
				char str[MAX];
				FILE *fptr1, *fptr2;
				int lno, linectr =0;
				log_info(logger,"Block edita: %s\n",rutaBlock);
				fptr1 = fopen(rutaBlock, "r");
				fptr2 = fopen(temp, "w+");
				lno = 3;
				while (!feof(fptr1))
				{
					strcpy(str, "\0");
					fgets(str,MAX,fptr1);
					if(!feof(fptr1))
					{
						linectr++;
						if(linectr !=lno)
						{
							fprintf(fptr2, "%s", str);
						}else{
							fprintf(fptr2, "%s", newln);
						}
					}
				}
				fclose(fptr1);
				fclose(fptr2);
				if(remove(rutaBlock) !=0){
					log_error(logger,"Fallo al edicion del block %s.", rutaBlock);
				}
				if(rename(temp, rutaBlock) !=0){
					log_error(logger,"Fallo al edicion del block %s.", rutaBlock);
				}
			}else{
				log_error(logger,"No existe la posicion %s.", pokemon);
				return -1;
			}
		}
	}
	return 0;
}


void* recvMensajesGameCard(void* socketCliente) {
	printf("Rompo en recvmensajes 0\n");
	int socket = *(int*) socketCliente;
	printf("Hilo para recibir mensajes del socket %d\n",socket);

	printf("Rompo en recvmensajes 1\n");
	t_paquete* bufferLoco = malloc(sizeof(t_paquete));
	while (1) {
		printf("Rompo en recvmensajes 2\n");
		printf("Estoy por recibir mensaje!\n");
		bufferLoco = recibirMensaje(socket);
		printf("Rompo en recvmensajes 3\n");
		pthread_mutex_lock(&mutex_bandejaGameCard);
		printf("Rompo en recvmensajes 4\n");
		queue_push(bandejaDeMensajesGameCard, (void*) bufferLoco);
		printf("CARBON 4\n");
		pthread_mutex_unlock(&mutex_bandejaGameCard);
		printf("UTNSO \n");
		sem_post(&contadorBandejaGameCard);

		printf("Rompo en recvmensajes 5\n");
	}

	return NULL;

}


void* procesarMensajeGameCard()
{
	// aca , la idea es saber que pokemon ponemos en el mapa por ejemplo.

	printf("Rompo en procesarMensaje 1\n");
	t_paquete* bufferLoco = malloc(sizeof(t_paquete));
	printf("CREO SOCKET CON EL BORKER 1\n");
	int socketBroker;

	socketBroker=crearConexion(gameCardConfig->ipBroker,gameCardConfig->puertoBroker,gameCardConfig->tiempoReintentoConexion);

	while(1){
		printf("Rompo en procesarMensaje 2\n");
		sem_wait(&contadorBandejaGameCard);
		pthread_mutex_lock(&mutex_bandejaGameCard);
		printf("Rompo en procesarMensaje 3\n");
		bufferLoco = (t_paquete*) queue_pop(bandejaDeMensajesGameCard); //ver en que posicion busco, por ahi se necesita una variable.
		printf("Rompo en procesarMensaje 4\n");
		pthread_mutex_unlock(&mutex_bandejaGameCard);
		printf("Entro al SWITCH 1\n");
		switch (bufferLoco->codigoOperacion) {
		case MENSAJE_NEW_POKEMON:
		{ 	//ver que casos usa el team
			printf("ENTRE por NEW_POKEMON envio appeared \n");

			//agregarNewPokemon(bufferLoco->buffer->nombrePokemon, bufferLoco->buffer->posX,bufferLoco->buffer->posY, bufferLoco->buffer->cantidadPokemons);

			//Si , envio mensaje al broker usando funcion del teeam
			//enviarMensajeTeamAppeared("pikachu",5,6,socketBroker);
			break;
		}
		case MENSAJE_GET_POKEMON: {
			printf("ENTRE POR GET_POKEMON Envio LOCALIZED \n");

			//Segmentationfault
			//enviarMensajeLocalized("Pikachu",t_coordenadas,socketBroker);

			break;
		}
		case MENSAJE_APPEARED_POKEMON: {

			break;
										}
		case MENSAJE_CATCH_POKEMON:{
			printf("ENTRE EN EL CATCH EENVIO CAUGHT a BROKER");

			enviarMensajeBrokerCaught(4,1,socketBroker);
			break;

									}
		default:
		{
			break;
		}

		printf("Rompo en procesarMensaje 5\n");
						}

		}
	printf("Estoy afuera del while \n");
	return NULL;
}

void inicializarMutexGameCard() {
	pthread_mutex_init(&mutex_bandejaGameCard,NULL);
	pthread_mutex_init(&mutex_crear_carpeta,NULL);
	pthread_mutex_init(&mutex_cant_blockers,NULL);
	sem_init(&contadorBandejaGameCard,1,0);
	return;
}

