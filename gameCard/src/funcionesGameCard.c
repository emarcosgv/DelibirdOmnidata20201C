#include "gameCard.h"

void inicializar_logger() {
	logger = log_create("GAMECARD.log", "GAMECARD", 1, LOG_LEVEL_TRACE);
	if (logger == NULL) {
		perror("No se puso inicializar el logger\n");
		exit(1);
	}
}

void cargarConfigGameCard() {
	gameCardConfig = (t_GAMECARDConfig*) malloc(sizeof(t_GAMECARDConfig));

	GAMECARDTConfig = config_create(GAMECARD_CONFIG_PATH);
	if (GAMECARDTConfig == NULL) {
		perror("Error al abrir el archivo de configuracion");
		log_error(logger, "- NO SE PUDO IMPORTAR LA CONFIGURACION");
		exit(2);
	}

	gameCardConfig->tiempoReintentoConexion = config_get_int_value(
			GAMECARDTConfig, "TIEMPO_DE_REINTENTO_CONEXION");
	gameCardConfig->tiempoReintentoOperacion = config_get_int_value(
			GAMECARDTConfig, "TIEMPO_DE_REINTENTO_OPERACION");
	gameCardConfig->ipBroker = string_duplicate(
			config_get_string_value(GAMECARDTConfig, "IP_BROKER"));
	gameCardConfig->puertoBroker = config_get_int_value(GAMECARDTConfig,
			"PUERTO_BROKER");
	gameCardConfig->puntoDeMontaje = string_duplicate(
			config_get_string_value(GAMECARDTConfig,
					"PUNTO_MONTAJE_TALLGRASS"));
	gameCardConfig->ipGameCard = string_duplicate(
			config_get_string_value(GAMECARDTConfig, "IP_GAMECARD"));
	gameCardConfig->puertoGameCard = config_get_int_value(GAMECARDTConfig,
			"PUERTO_GAMECARD");
	gameCardConfig->nombreProceso = string_duplicate(
			config_get_string_value(GAMECARDTConfig, "NOMBRE_PROCESO"));

	log_info(logger, "- tiempoReintentoConexion=%d\n",
			gameCardConfig->tiempoReintentoConexion);
	log_info(logger, "- tiempoReintentoOperacion=%d\n",
			gameCardConfig->tiempoReintentoOperacion);
	log_info(logger, "- puertoBroker=%d\n", gameCardConfig->puertoBroker);
	log_info(logger, "- ipBroker=%s\n", gameCardConfig->ipBroker);
	log_info(logger, "- puntoDeMontaje=%s\n", gameCardConfig->puntoDeMontaje);
	log_info(logger, "- ipGameCard=%s\n", gameCardConfig->ipGameCard);
	log_info(logger, "- puertoGameCard\n", gameCardConfig->puertoGameCard);
	log_info(logger, "- nombreProceso\n", gameCardConfig->nombreProceso);

	log_info(logger, "- CONFIG IMPORTADA CON EXITO\n");
	return;
}

void suscribirmeAColasBroker() {
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
void actualizarBlocks() {
	char* ruta_metadata = string_new();
	string_append(&ruta_metadata, gameCardConfig->puntoDeMontaje);
	string_append(&ruta_metadata, RUTA_METADATA_GENERAL);

	char* ruta_block = string_new();
	string_append(&ruta_block, gameCardConfig->puntoDeMontaje);
	string_append(&ruta_block, "/Blocks/");

	char buff2[255];

	log_info(logger,
			"En la ruta %s se encuentra la metadata general del Tall Grass\n",
			ruta_metadata);

	FILE *fp = fopen(ruta_metadata, "r");

	//BLOCK_SIZE=64
	fscanf(fp, "%s", buff2);
	//BLOCKS=5192
	fscanf(fp, "%s", buff2);
	char* linea_blocks = string_duplicate(buff2);
	fclose(fp);

	char** linea_blocks_array = string_split(linea_blocks, "=");
	string_trim(linea_blocks_array);

	sscanf(linea_blocks_array[1], "%d", &blocks_maximos);
	if (blocks_maximos == 0) {
		log_error(logger, "No se pudo cargar la metadata");
	}

	// Existen blocks en la carpeta
	int i_aux = 0;
	char* direccion_aux = string_duplicate(ruta_block);
	for (int i = 1; i <= blocks_maximos; i++) {

		string_append(&direccion_aux, string_itoa(i));
		string_append(&direccion_aux, ".bin");

		if (access(direccion_aux, F_OK) != -1) {
			i_aux++;
		}
	}
	free(direccion_aux);
	blocks_usados = i_aux;
	log_info(logger, "Ya existen %d blocks en TallGrass", blocks_usados);
	free(ruta_metadata);
	free(ruta_block);
	free(linea_blocks);
	return;
}

char* crearRutaArchivo(char* nombreArchivo) {
	char* rutaArchivo = string_new();
	string_append(&rutaArchivo, gameCardConfig->puntoDeMontaje);
	string_append(&rutaArchivo, nombreArchivo);
	log_debug(logger, "Ruta creada: %s", rutaArchivo);
	return rutaArchivo;
}

char* crearRutaPokemon(char* nombrePokemon) {
	char* rutaArchivo = string_new();
	string_append(&rutaArchivo, gameCardConfig->puntoDeMontaje);
	string_append(&rutaArchivo, "/Files/Pokemon/");
	string_append(&rutaArchivo, nombrePokemon);
	string_append(&rutaArchivo, "/Metadata.bin");
	return rutaArchivo;
}

void escribir_archivo(char* rutaArchivo, char* stringAEscribir) {
	FILE *fp = txt_open_for_append(rutaArchivo);

	if (fp == NULL) {
		log_error(logger, "Error al crear archivo %s\n", rutaArchivo);
		exit(1);
	}
	txt_write_in_file(fp, stringAEscribir);
	txt_close_file(fp);

	log_info(logger, "ARCHIVO %s ACTUALIZADO\n", rutaArchivo);
	return;
}

void terminarProgramaGameCard() {
	log_destroy(logger);
	config_destroy(GAMECARDTConfig);
	free(gameCardConfig);
}

void crearCarpeta(char* ruta) {
	struct stat st = { 0 };
	if (stat(ruta, &st) == -1) {
		//No tengo idea si los permisos 0777 son seguros

		pthread_mutex_lock(&mutex_crear_carpeta);
		int exito = mkdir(ruta, 0777);
		pthread_mutex_unlock(&mutex_crear_carpeta);
		if (exito == -1) {
			log_error(logger, "No se pudo crear la carpeta: %s", ruta);
		}
	}
}

void iniciarTallGrass() {
	crearCarpeta(gameCardConfig->puntoDeMontaje);

	char* ruta_metadata = string_new();
	string_append(&ruta_metadata, gameCardConfig->puntoDeMontaje);
	string_append(&ruta_metadata, "/Metadata");
	crearCarpeta(ruta_metadata);

	char* ruta_blocks = string_new();
	string_append(&ruta_blocks, gameCardConfig->puntoDeMontaje);
	string_append(&ruta_blocks, "/Blocks");
	crearCarpeta(ruta_blocks);

	char* ruta_files = string_new();
	string_append(&ruta_files, gameCardConfig->puntoDeMontaje);
	string_append(&ruta_files, "/Files");
	crearCarpeta(ruta_files);

	char* ruta_pokemon = string_new();
	string_append(&ruta_pokemon, gameCardConfig->puntoDeMontaje);
	string_append(&ruta_pokemon, "/Files");
	string_append(&ruta_pokemon, "/Pokemon");
	crearCarpeta(ruta_pokemon);

	// Crear archivos Metadata general
	char* rutaMetadata = crearRutaArchivo(RUTA_METADATA_GENERAL);
	crear_bitmap();
	//char* rutaBitmap = crearRutaArchivo(RUTA_BITMAP_GENERAL);


	//if (access(rutaMetadata, F_OK) == -1) {escribir_archivo(rutaBitmap, "\n");
	//}



	if (access(rutaMetadata, F_OK) == -1) {
		char* linea_metadata = string_new();
		string_append(&linea_metadata, "BLOCK_SIZE=64\n");
		string_append(&linea_metadata, "BLOCKS=5192\n");
		string_append(&linea_metadata, "MAGIC_NUMBER=TALL_GRASS\n");

		escribir_archivo(rutaMetadata, linea_metadata);
		free(linea_metadata);
	}
	free(ruta_metadata);
	free(ruta_blocks);
	free(ruta_files);
	free(ruta_pokemon);
	free(rutaMetadata);
	//free(rutaBitmap);
	return;
}

int archivoAbierto(char* rutaArchivo) {
	/*
	 *  true   1
	 *  false  0
	 */
	return 0;
}

int existePokemon(char* rutaPokemon) {
	/*
	 *  true   1
	 *  false  0
	 */
	if (access(rutaPokemon, F_OK) != -1) {
		// Existe el pokemon
		return 1;
	} else {
		// No existe el pokemon
		return 0;
	}
}

int crearBlock(int block, int x, int y, int cant) {
	char* rutaBlocks = string_new();
	string_append(&rutaBlocks, gameCardConfig->puntoDeMontaje);
	string_append(&rutaBlocks, "/Blocks/");
	string_append(&rutaBlocks, string_itoa(block));
	string_append(&rutaBlocks, ".bin");
	//char* ruta=crearRutaArchivo(rutaBlocks);

	char* str_block = string_new();
	string_append(&str_block, "X=");
	string_append(&str_block, string_itoa(x));
	string_append(&str_block, "\n");

	string_append(&str_block, "Y=");
	string_append(&str_block, string_itoa(y));
	string_append(&str_block, "\n");

	string_append(&str_block, "CANTIDAD=");
	string_append(&str_block, string_itoa(cant));
	string_append(&str_block, "\n");

	escribir_archivo(rutaBlocks, str_block);
	free(str_block);

	struct stat st;
	stat(rutaBlocks, &st);
	int size = st.st_size;

	log_info(logger, "Block %s creado - size %d", rutaBlocks, size);
	free(rutaBlocks);
	return size;
}

t_paquete* obtenerPokemon(char* pokemon) {
	t_posicion *XY = malloc(sizeof(t_posicion));
	t_list *CoordXY = list_create();
	t_paquete *bufferLoco;
	bufferLoco = malloc(sizeof(t_paquete));
	bufferLoco->buffer = malloc(sizeof(t_bufferOmnidata));

	bufferLoco->buffer->listaCoordenadas = list_create();
	int cantidad = 0;
	char* rutaPokemon = crearRutaPokemon(pokemon);

	char* carpetaPokemon = string_new();
	string_append(&carpetaPokemon, gameCardConfig->puntoDeMontaje);
	string_append(&carpetaPokemon, "/Files/Pokemon/");
	string_append(&carpetaPokemon, pokemon);

	if (existePokemon(rutaPokemon) == 1) //NO EXISTE EL POKEMON
			{

		log_info(logger, "existe el pokemon");

		char buff[255];
		FILE *fp = fopen(rutaPokemon, "r");
		// Linea OPEN
		fscanf(fp, "%s", buff);
		// Pasamos la primera linea
		fscanf(fp, "%s", buff);
		// Linea SIZE
		fscanf(fp, "%s", buff);
		char* ln_size_actual = string_duplicate(buff);
		char** size_array = string_split(ln_size_actual, "=");
		int int_size_actual;
		string_trim(size_array);
		sscanf(size_array[1], "%d", &int_size_actual);
		free(ln_size_actual);

		fscanf(fp, "%s", buff);
		char** block_array = string_split(buff, "=");
		string_trim(block_array);

		fclose(fp);

		// restamos los []

		char* newln2 = string_new();
		string_append(&newln2, "BLOCKS=[");

		char aux[100];
		char** array_strings = string_get_string_as_array(block_array[1]);
		// Itearamos para saber si existen las mismas coordenadas en el filesystem
		// Si es el caso, aumentamos solo la cantidad dentro del block y terminamos.

		while (*array_strings != NULL) {

			char buff2[255];

			strcpy(aux, *array_strings);

			char* a_ruta = string_duplicate(aux);
			char* a_newln2 = string_duplicate(aux);

			char* ruta = string_new();
			string_append(&ruta, gameCardConfig->puntoDeMontaje);
			string_append(&ruta, "/Blocks/");
			//string_append(&ruta,array_strings[i]);
			string_append(&ruta, a_ruta);
			string_append(&ruta, ".bin");

			string_append(&newln2, a_newln2);
			string_append(&newln2, ",");

			FILE *fp_block = fopen(ruta, "r");

			// POSX
			fscanf(fp_block, "%s", buff2);
			printf("1: %s\n", buff2);
			char* s_x = strdup(buff2);
			char** block_arrayx = string_split(s_x, "=");
			int int_x;
			string_trim(block_arrayx);
			sscanf(block_arrayx[1], "%d", &int_x);
			XY->x = int_x;

			//list_add(CoordXY,(void*)int_x);
			//block_arrayx[1]

			//POSY
			fscanf(fp_block, "%s", buff2);
			printf("2: %s\n", buff2);
			char* s_y = string_duplicate(buff2);
			char** block_arrayy = string_split(s_y, "=");
			int int_y;
			string_trim(block_arrayy);
			sscanf(block_arrayy[1], "%d", &int_y);
			XY->y = int_y;
			//list_add(CoordXY,(void*)int_y);
			list_add(CoordXY, (void*) XY);

			//CANTIDAD
			fscanf(fp_block, "%s", buff2);
			printf("3: %s\n", buff2);
			fscanf(fp_block, "%s", buff2);
			char* s_cant = string_duplicate(buff2);
			char** block_arraycant = string_split(s_cant, "=");
			int int_cant;
			string_trim(block_arraycant);
			sscanf(block_arraycant[1], "%d", &int_cant);

			cantidad = +int_cant;

			fclose(fp_block);
			array_strings++;

			free(s_cant);
			free(s_y);
			free(s_x);
			free(ruta);

		}
		free(block_array);

		bufferLoco->buffer->listaCoordenadas = list_duplicate(CoordXY);
		bufferLoco->buffer->nombrePokemon = pokemon;
		bufferLoco->buffer->cantidadPokemons = cantidad;
		list_destroy(CoordXY);

	}

	return bufferLoco;
}

char* crearRutaCarpetaPokemon(char* pokemon) {
	char* carpetaPokemon = string_new();
	string_append(&carpetaPokemon, gameCardConfig->puntoDeMontaje);
	string_append(&carpetaPokemon, "/Files/Pokemon/");
	string_append(&carpetaPokemon, pokemon);
	return carpetaPokemon;
}

void crearPokemonDesdeCero(char* pokemon, int x, int y, int cantidad) {
	char* rutaPokemon = crearRutaPokemon(pokemon);
	char* rutaCarpetaPokemon = crearRutaCarpetaPokemon(pokemon);
	crearCarpeta(rutaCarpetaPokemon);

	pthread_mutex_lock(&mutex_cant_blockers);
	blocks_usados++;
	pthread_mutex_unlock(&mutex_cant_blockers);

	int size = crearBlock(blocks_usados, x, y, cantidad);
	char* linea1Metadata = string_new();

	string_append(&linea1Metadata, "OPEN=N");
	string_append(&linea1Metadata, "\n");
	string_append(&linea1Metadata, "DIRECTORY=N\n");
	string_append(&linea1Metadata, "SIZE=");
	string_append(&linea1Metadata, string_itoa(size));
	string_append(&linea1Metadata, "\n");
	string_append(&linea1Metadata, "BLOCKS=[");
	string_append(&linea1Metadata, string_itoa(blocks_usados));
	string_append(&linea1Metadata, "]");
	string_append(&linea1Metadata, "\n");

	escribir_archivo(rutaPokemon, linea1Metadata);
	free(rutaCarpetaPokemon);
	free(linea1Metadata);
	free(rutaPokemon);
	return;
}

void agregarNewPokemon(char* pokemon, int x, int y, int cantidad) {

	//printf("Se agregan %d %s en (%d, %d)\n", cantidad, pokemon, x, y);
	char* rutaPokemon = crearRutaPokemon(pokemon);

	if (existePokemon(rutaPokemon) == 0) {			//NO EXISTE EL POKEMON
		crearPokemonDesdeCero(pokemon, x, y, cantidad);
		free(rutaPokemon);
		return;
	} else { // EXISTE EL POKEMON

		log_info(logger, "SE ENCONTRO EL POKEMON %s EN TAILGRASS", pokemon);
		int flag = 1;

		char* open = "OPEN=Y";

		while (flag) {
			char buff[255];
			FILE *fp = fopen(rutaPokemon, "r");

			fscanf(fp, "%s", buff);
			char* estado = string_duplicate(buff);
			int indice = fileno(fp);
			flock(indice, LOCK_EX);

			if (strcmp(estado, open) == 0) {

				log_error(logger, "El archivo %s ya esta abierto", rutaPokemon);

				flock(indice, LOCK_UN);

				sleep(gameCardConfig->tiempoReintentoOperacion);
				//printf("Estoy en el sleep\n");
			} else {
//				int indice = fileno(fp);
//				flock(indice,LOCK_SH);

				ArchivoEnUso(rutaPokemon, pokemon);
				sleep(1);
				flock(indice, LOCK_UN);
				printf("Lo estoy USANDO\n");

				// Pasamos la primera linea
				fscanf(fp, "%s", buff);
				// Linea SIZE
				fscanf(fp, "%s", buff);
				char* ln_size_actual = string_duplicate(buff);
				char** size_array = string_split(ln_size_actual, "=");
				int int_size_actual;
				string_trim(size_array);
				sscanf(size_array[1], "%d", &int_size_actual);
				free(ln_size_actual);

				fscanf(fp, "%s", buff);
				char** block_array = string_split(buff, "=");
				string_trim(block_array);

				fclose(fp);

				// restamos los []

				char* newln2 = string_new();
				string_append(&newln2, "BLOCKS=[");

				char aux[100];
				char** array_strings = string_get_string_as_array(
						block_array[1]);

				// Itearamos para saber si existen las mismas coordenadas en el filesystem
				// Si es el caso, aumentamos solo la cantidad dentro del block y terminamos.
				int i = 0;
				while (*array_strings != NULL) {
					int mismaposicion = 0;
					char buff2[255];

					strcpy(aux, *array_strings);

					char* a_ruta = string_duplicate(aux);
					char* a_newln2 = string_duplicate(aux);

					char* ruta = string_new();
					string_append(&ruta, gameCardConfig->puntoDeMontaje);
					string_append(&ruta, "/Blocks/");
					//string_append(&ruta,array_strings[i]);
					string_append(&ruta, a_ruta);
					string_append(&ruta, ".bin");

					string_append(&newln2, a_newln2);
					string_append(&newln2, ",");

					FILE *fp_block = fopen(ruta, "r");

					// POSX
					fscanf(fp_block, "%s", buff2);
					char* s_x = strdup(buff2);
					char** block_arrayx = string_split(s_x, "=");
					string_trim(block_arrayx);
					int int_x;
					sscanf(block_arrayx[1], "%d", &int_x);
					if (x == int_x) {
						mismaposicion++;
					}

					//POSY
					fscanf(fp_block, "%s", buff2);
					char* s_y = string_duplicate(buff2);
					char** block_arrayy = string_split(s_y, "=");
					int int_y;
					string_trim(block_arrayy);
					sscanf(block_arrayy[1], "%d", &int_y);
					if (int_y == y) {
						mismaposicion++;
					};

					//CANTIDAD
					fscanf(fp_block, "%s", buff2);
					//printf("3: %s\n", buff2);
					fscanf(fp_block, "%s", buff2);
					char* s_cant = string_duplicate(buff2);
					char** block_arraycant = string_split(s_cant, "=");
					int int_cant;
					string_trim(block_arraycant);
					sscanf(block_arraycant[1], "%d", &int_cant);
					int cantidad_actualizada = cantidad + int_cant;

					fclose(fp_block);
					array_strings++;
					i++;
					if (mismaposicion == 2)	// Se encontraron las coordenadas.
							{
						char* ruta_blocks = string_new();
						string_append(&ruta_blocks,
								gameCardConfig->puntoDeMontaje);
						string_append(&ruta_blocks, "/Blocks/temp.txt");
						char* temp = string_duplicate(ruta_blocks);

						char* newln = string_new();
						string_append(&newln, "CANTIDAD=");
						string_append(&newln,
								string_itoa(cantidad_actualizada));
						string_append(&newln, "\n");

						int MAX = 256;
						char str[MAX];
						// En la linea 3 se encuentra la CANTIDAD
						int lno = 3;
						int linectr = 0;

						FILE *fptr1 = fopen(ruta, "r");
						FILE *fptr2 = fopen(temp, "w+");

						while (!feof(fptr1)) {
							strcpy(str, "\0");
							fgets(str, MAX, fptr1);
							if (!feof(fptr1)) {
								linectr++;
								if (linectr != lno) {
									fprintf(fptr2, "%s", str);
								} else {
									fprintf(fptr2, "%s", newln);
								}
							}
						}
						fclose(fptr1);
						fclose(fptr2);

						if (remove(ruta) == -1) {
							//printf("No funciono el remove");
						}
						if (rename(temp, ruta)) {
							//printf("No funciono el rename");
						}

						log_info(logger,
								"Cantidad %d actualizada en block %s para pokemon %s",
								cantidad_actualizada, ruta, pokemon);

						//pthread_mutex_lock(&mutex_archivo);

						ArchivoAbiertoParaUso(rutaPokemon, pokemon);
						flock(indice, LOCK_EX);
						flag = 0;
						//pthread_mutex_lock(&mutex_archivo);

						free(block_array);
						free(s_cant);
						free(s_y);
						free(s_x);
						free(ruta);

						free(ruta_blocks);
						free(newln);
						free(newln2);
						return;
					}

					free(s_cant);
					free(s_y);
					free(s_x);
					free(ruta);
				}		//TERMINA EL WHILE DEL ARRAY

				free(block_array);

				// Crear nuevo block para el mismo pokemon

				pthread_mutex_lock(&mutex_cant_blockers);
				blocks_usados++;
				pthread_mutex_unlock(&mutex_cant_blockers);
				log_info(logger, "CREANDO BLOCK NRO %d PARA %s", blocks_usados,
						pokemon);

				int size_block_nw = crearBlock(blocks_usados, x, y, cantidad);

				char* dir_pokemon = string_new();
				string_append(&dir_pokemon, gameCardConfig->puntoDeMontaje);
				string_append(&dir_pokemon, "/Files/Pokemon/");
				string_append(&dir_pokemon, pokemon);
				string_append(&dir_pokemon, "temp.txt");

				/*
				 for(int i=0; i<blocks_totales; i++)
				 {
				 string_append(&newln2, array_strings[i]);
				 string_append(&newln2,",");
				 }*/

				string_append(&newln2, string_itoa(blocks_usados));
				string_append(&newln2, "]");
				string_append(&newln2, "\n");

				int size_actualizadp = size_block_nw + int_size_actual;

				char* linea_size = string_new();

				string_append(&linea_size, "SIZE=");
				string_append(&linea_size, string_itoa(size_actualizadp));
				string_append(&linea_size, "\n");

				int MAX = 256;
				char str[MAX];

				FILE *fptr1 = fopen(rutaPokemon, "r");
				FILE *fptr2 = fopen(dir_pokemon, "w+");

				int lno_blocks = 4;
				int lno_size_blocks = 3;
				int linectr = 0;

				log_info(logger, "Linea a ser escrita en la posicion %d: %s",
						lno_size_blocks, linea_size);
				log_info(logger, "Linea a ser escrita en la posicion %d: %s",
						lno_blocks, newln2);

				while (!feof(fptr1)) {
					strcpy(str, "\0");
					fgets(str, MAX, fptr1);
					if (!feof(fptr1)) {
						linectr++;
						if (linectr == lno_size_blocks) {
							fprintf(fptr2, "%s", linea_size);
						} else if (linectr == lno_blocks) {
							fprintf(fptr2, "%s", newln2);

						} else {
							fprintf(fptr2, "%s", str);
						}
					}
				}
				fclose(fptr1);
				fclose(fptr2);
				remove(rutaPokemon);

				rename(dir_pokemon, rutaPokemon);

				free(linea_size);
				free(newln2);
				free(dir_pokemon);

				//pthread_mutex_lock(&mutex_archivo);

				ArchivoAbiertoParaUso(rutaPokemon, pokemon);
				flock(indice, LOCK_EX);
				flag = 0;
				//pthread_mutex_unlock(&mutex_archivo);

			}
			//else {

			//log_error(logger, "archivo en uso");
			//printf("archivo en uso!!'\n");

		}
	}

	free(rutaPokemon);
	return;
}

int catchPokemon(char* pokemon, int x, int y) {
	log_info(logger, "CATCH Pokemon %s", pokemon);

	char* carpetaPokemon = string_new();
	string_append(&carpetaPokemon, gameCardConfig->puntoDeMontaje);
	string_append(&carpetaPokemon, "/Files/Pokemon/");
	string_append(&carpetaPokemon, pokemon);
	char* rutaPokemon = crearRutaPokemon(pokemon);

	if (existePokemon(rutaPokemon) == 0) {
		perror("El pokemon no existe");
		log_error(logger, "- El pokemon %s no existe", pokemon);
		return -1;
	} else {
		// 1. Verificar si las posiciones existen

		int flag = 1;

		char* open = "OPEN=Y";

		while (flag) {
			char buff[255];
			FILE *fp = fopen(rutaPokemon, "r");

			fscanf(fp, "%s", buff);
			char* estado = string_duplicate(buff);
			int indice = fileno(fp);
			flock(indice, LOCK_EX);

			if (strcmp(estado, open) == 0) {
				log_error(logger, "El archivo %s ya esta abierto", rutaPokemon);

				flock(indice, LOCK_UN);

				sleep(gameCardConfig->tiempoReintentoOperacion);
			} else {

				ArchivoEnUso(rutaPokemon, pokemon);
				sleep(1);
				flock(indice, LOCK_UN);
				printf("Lo estoy USANDO\n");

				// Linea directory
				fscanf(fp, "%s", buff);
				// Linea size
				fscanf(fp, "%s", buff);
				char* ln_size_actual = string_duplicate(buff);
				char** size_array = string_split(ln_size_actual, "=");
				int int_size_actual;
				string_trim(size_array);
				sscanf(size_array[1], "%d", &int_size_actual);
				// Linea 3
				fscanf(fp, "%s", buff);
				fclose(fp);
				char** block_array = string_split(buff, "=");
				string_trim(block_array);

				// restamos los []
				int cant_numeros = string_length(block_array[1]) - 2;

				char** array_strings = string_get_string_as_array(
						block_array[1]);

				for (int i = 0; i < cant_numeros; i++) {
					int mismaposicion = 0;
					char* rutaBlocks = string_new();
					string_append(&rutaBlocks, "/Blocks/");
					string_append(&rutaBlocks, array_strings[i]);
					string_append(&rutaBlocks, ".bin");
					char* rutaBlock = crearRutaArchivo(rutaBlocks);

					char buff2[255];
					fp = fopen(rutaBlock, "r");
					fscanf(fp, "%s", buff2);

					char* s_x = string_duplicate(buff2);
					char** block_arrayx = string_split(s_x, "=");
					string_trim(block_arrayx);

					int int_x;
					sscanf(block_arrayx[1], "%d", &int_x);
					if (x == int_x) {
						mismaposicion++;
					}

					fscanf(fp, "%s", buff2);
					char* s_y = string_duplicate(buff2);
					char** block_arrayy = string_split(s_y, "=");
					int int_y;
					string_trim(block_arrayy);
					sscanf(block_arrayy[1], "%d", &int_y);

					if (int_y == y) {
						mismaposicion++;
					};
					fscanf(fp, "%s", buff2);
					fscanf(fp, "%s", buff2);

					char* s_cant = string_duplicate(buff2);
					char** block_arraycant = string_split(s_cant, "=");
					int int_cant;
					string_trim(block_arrayy);
					sscanf(block_arraycant[1], "%d", &int_cant);
					if (int_cant == 0) {

						ArchivoAbiertoParaUso(rutaPokemon, pokemon);
						flock(indice, LOCK_EX);
						flag = 0;
						return -1;
					}
					fclose(fp);
					if (mismaposicion == 2) {
						char* ruta_blocks = string_new();
						string_append(&ruta_blocks,
								gameCardConfig->puntoDeMontaje);
						string_append(&ruta_blocks, "/Blocks/temp.txt");

						char* temp = string_duplicate(ruta_blocks);

						int cantidad_actualizada = int_cant - 1;
						char* newln = string_new();
						string_append(&newln, "CANTIDAD=");
						string_append(&newln,
								string_itoa(cantidad_actualizada));
						string_append(&newln, "\n");

						int MAX = 256;
						char str[MAX];
						FILE *fptr1, *fptr2;
						int lno, linectr = 0;
						log_info(logger, "Block edita: %s\n", rutaBlock);
						fptr1 = fopen(rutaBlock, "r");
						fptr2 = fopen(temp, "w+");
						lno = 3;
						while (!feof(fptr1)) {
							strcpy(str, "\0");
							fgets(str, MAX, fptr1);
							if (!feof(fptr1)) {
								linectr++;
								if (linectr != lno) {
									fprintf(fptr2, "%s", str);
								} else {
									fprintf(fptr2, "%s", newln);
								}
							}
						}
						fclose(fptr1);
						fclose(fptr2);
						if (remove(rutaBlock) != 0) {
							log_error(logger, "Fallo al edicion del block %s.",
									rutaBlock);
						}
						if (rename(temp, rutaBlock) != 0) {
							log_error(logger, "Fallo al edicion del block %s.",
									rutaBlock);
						}
					} else {
						log_error(logger, "No existe la posicion %s.", pokemon);
						ArchivoAbiertoParaUso(rutaPokemon, pokemon);
						flag = 0;
						return -1;
					}
				}

				ArchivoAbiertoParaUso(rutaPokemon, pokemon);
				flock(indice, LOCK_EX);
				flag = 0;
			}
		}
	}

	return 0;
}

void* recvMensajesGameCard(void* socketCliente) {
	printf("Inicio el recv mensaje GameCard\n");

	int socket = *(int*) socketCliente;

	t_paquete* bufferLoco = malloc(sizeof(t_paquete));
	int flag = 1;
	while (flag) {

		bufferLoco = recibirMensaje(socket);

		if (bufferLoco != NULL) {
			//pthread_t hilito2;
			//pthread_create(&hilito2, NULL, auxiliar2, (void*) bufferLoco);
			pthread_mutex_lock(&mutex_bandejaGameCard);
			printf("AGREGUE A LA COLA\n");
			queue_push(bandejaDeMensajesGameCard, (void*) bufferLoco);
			pthread_mutex_unlock(&mutex_bandejaGameCard);
			//sem_post(&contadorBandejaGameCard);
			sem_post(&bandejaCounter);

			//pthread_detach(hilito2);

		} else
			flag = 0;
		//procesarMensajeGameCard();

	}
	printf("Termino el recv mensaje GameCard\n");

	free(bufferLoco);
	return NULL;

}

void* procesarMensajeGameCard() {

	printf("Hilo asignado para procesar mensajes\n");

	t_paquete* bufferLoco = malloc(sizeof(t_paquete));


	pthread_mutex_lock(&mutex_bandejaGameCard);
	printf("Sque de la cola \n");
	bufferLoco = (t_paquete*) queue_pop(bandejaDeMensajesGameCard);

	pthread_mutex_unlock(&mutex_bandejaGameCard);

//Creo una lista de ID_MESAJES

//bufferLoco->buffer->idMensajeCorrelativo;
//ListIdMensajes =list_create();
//list_add(ListIdMensajes, (void*)bufferLoco->buffer->idMensajeCorrelativo);

	switch (bufferLoco->codigoOperacion) {
	case MENSAJE_NEW_POKEMON: {
		printf("ENTRE por NEW_POKEMON envio appeared \n");

		//printf("ENTRE AL HILO \n");
		//pthread_t hilito;
		//pthread_t hilitoPrueba;
		//pthread_create(&hilito, NULL, hiloAgregarPokemon, (void*) bufferLoco); //Llamar directo a la función.

		agregarNewPokemon(bufferLoco->buffer->nombrePokemon,
				bufferLoco->buffer->posX, bufferLoco->buffer->posY,
				bufferLoco->buffer->cantidadPokemons);

		int socketBroker;

		socketBroker=crearConexion(gameCardConfig->ipBroker,gameCardConfig->puertoBroker,gameCardConfig->tiempoReintentoConexion);

		if (socketBroker > 0) {

			enviarMensajeBrokerAppeared(bufferLoco->buffer->nombrePokemon,
					bufferLoco->buffer->posX, bufferLoco->buffer->posY,
					bufferLoco->buffer->idMensajeCorrelativo, socketBroker);
		}

		printf("Temine de agregar Pokemon, Esperando otro mensaje \n");

		break;
	}
	case MENSAJE_GET_POKEMON: {
		printf("ENTRE POR GET_POKEMON Envio LOCALIZED al BROKER \n");
		printf("Id mensaje = %d, Id correlativo =%d\n",bufferLoco->buffer->idMensaje,bufferLoco->buffer->idMensajeCorrelativo);
		int idCorrelativo = bufferLoco->buffer->idMensaje;
		bufferLoco = obtenerPokemon(bufferLoco->buffer->nombrePokemon);
		bufferLoco->buffer->idMensajeCorrelativo=idCorrelativo;
		int socketBroker;

			socketBroker=crearConexion(gameCardConfig->ipBroker,gameCardConfig->puertoBroker,gameCardConfig->tiempoReintentoConexion);

		if (socketBroker > 0) {

		//enviarMensajeLocalized(bufferLoco->buffer->nombrePokemon,bufferLoco->buffer->listaCoordenadas,socketBroker);
		enviarMensajeLocalizedId(bufferLoco->buffer->nombrePokemon,bufferLoco->buffer->listaCoordenadas,bufferLoco->buffer->idMensajeCorrelativo,socketBroker);
		}
		break;

	}
	case MENSAJE_CATCH_POKEMON: {
		printf("ENTRE EN EL CATCH EENVIO CAUGHT a BROKER");

		int resultado = catchPokemon(bufferLoco->buffer->nombrePokemon,
				bufferLoco->buffer->posX, bufferLoco->buffer->posY);
		int socketBroker;

				socketBroker=crearConexion(gameCardConfig->ipBroker,gameCardConfig->puertoBroker,gameCardConfig->tiempoReintentoConexion);


		if (resultado == -1) {
			log_error(logger, "No se pudo atrapar");
			enviarMensajeBrokerCaught(bufferLoco->buffer->idMensaje, 0,
					socketBroker);
		} else {
			if (socketBroker > 0) {
			enviarMensajeBrokerCaught(bufferLoco->buffer->idMensaje, 1,
					socketBroker);
			}
		}
		break;

	}
	default: {
		break;
	}

		printf("Fuera del switch \n");
	}

//}
	printf("Termino el hilo procesoProcesarMensaje \n");
	free(bufferLoco);
	return NULL;
}

void inicializarMutexGameCard() {
	pthread_mutex_init(&mutex_bandejaGameCard, NULL);
	pthread_mutex_init(&mutex_crear_carpeta, NULL);
	pthread_mutex_init(&mutex_cant_blockers, NULL);
	sem_init(&contadorBandejaGameCard, 1, 0);
	return;
}

void* handlerGameCard(void* socketDelCliente) {
	recvMensajesGameCard(socketDelCliente);
	return NULL;
}

void *iniciarConexionBroker(void *arg) { //esta es una funcion que va a recibir el pthread_create
	socketBroker = crearConexion(gameCardConfig->ipBroker,
			gameCardConfig->puertoBroker,
			gameCardConfig->tiempoReintentoConexion);
	arg = (void*) socket;
	return NULL;
}

void ArchivoEnUso(char* rutaPokemon, char* pokemon) {

	char* dir_pokemon = string_new();
	string_append(&dir_pokemon, gameCardConfig->puntoDeMontaje);
	string_append(&dir_pokemon, "/Files/Pokemon/");
	string_append(&dir_pokemon, pokemon);
	string_append(&dir_pokemon, "temp.txt");

	char* linea_size1 = string_new();

	string_append(&linea_size1, "OPEN=Y");

	string_append(&linea_size1, "\n");

	int MAX = 256;
	char str[MAX];

	FILE *fptr1 = fopen(rutaPokemon, "r");
	FILE *fptr2 = fopen(dir_pokemon, "w+");

	int lno_size_blocks = 1;
	int linectr = 0;

	log_info(logger, "Metadata En USO %d: %s", lno_size_blocks, linea_size1);

	while (!feof(fptr1)) {
		strcpy(str, "\0");
		fgets(str, MAX, fptr1);
		if (!feof(fptr1)) {
			linectr++;
			if (linectr == lno_size_blocks) {
				fprintf(fptr2, "%s", linea_size1);
			} else {
				fprintf(fptr2, "%s", str);
			}
		}
	}
	fclose(fptr1);
	fclose(fptr2);
	remove(rutaPokemon);

	rename(dir_pokemon, rutaPokemon);

	free(linea_size1);

	free(dir_pokemon);

	return;
}

void ArchivoAbiertoParaUso(char* rutaPokemon, char* pokemon) {

	char* dir_pokemon = string_new();
	string_append(&dir_pokemon, gameCardConfig->puntoDeMontaje);
	string_append(&dir_pokemon, "/Files/Pokemon/");
	string_append(&dir_pokemon, pokemon);
	string_append(&dir_pokemon, "temp.txt");

	char* linea_size2 = string_new();

	string_append(&linea_size2, "OPEN=N");

	string_append(&linea_size2, "\n");

	int MAX = 256;
	char str[MAX];

	FILE *fptr1 = fopen(rutaPokemon, "r");
	FILE *fptr2 = fopen(dir_pokemon, "w+");

	int lno_size_blocks = 1;
	int linectr = 0;

	log_info(logger, "Linea a ser escrita en la posicion %d: %s",
			lno_size_blocks, linea_size2);

	while (!feof(fptr1)) {
		strcpy(str, "\0");
		fgets(str, MAX, fptr1);
		if (!feof(fptr1)) {
			linectr++;
			if (linectr == lno_size_blocks) {
				fprintf(fptr2, "%s", linea_size2);
			} else {
				fprintf(fptr2, "%s", str);
			}
		}
	}
	fclose(fptr1);
	fclose(fptr2);
	remove(rutaPokemon);

	rename(dir_pokemon, rutaPokemon);

	free(linea_size2);

	free(dir_pokemon);

	return;
}

void* hiloAgregarPokemon(void* bufferLoco1) {

	printf("SOY UN HILO AUXILIAR\n");

	t_paquete* bufferLoco2 = (t_paquete*) bufferLoco1;

	agregarNewPokemon(bufferLoco2->buffer->nombrePokemon,
			bufferLoco2->buffer->posX, bufferLoco2->buffer->posY,
			bufferLoco2->buffer->cantidadPokemons);

	return NULL;
}

void* auxiliar2(void* bufferLoco) {

	pthread_mutex_lock(&mutex_bandejaGameCard);
	printf("AGREGUE A LA COLA\n");
	queue_push(bandejaDeMensajesGameCard, (void*) bufferLoco);
	pthread_mutex_unlock(&mutex_bandejaGameCard);
//sem_post(&contadorBandejaGameCard);
	sem_post(&bandejaCounter);

	return NULL;

}

void* auxiliar3(void* bufferLoco) {

	sem_wait(&contadorBandejaGameCard);

	pthread_mutex_lock(&mutex_bandejaGameCard);
	bufferLoco = (t_paquete*) queue_pop(bandejaDeMensajesGameCard);
	pthread_mutex_unlock(&mutex_bandejaGameCard);
	if (bufferLoco != NULL) {

	} else {
		printf("ESTA VACIA \n");
	}

	return NULL;
}

void* escucharConexionesGameCard() {
	pthread_t threadId[MAX_CONEXIONES];

	int contadorConexiones = 0;
	int socketDelCliente[MAX_CONEXIONES];
	struct sockaddr direccionCliente;
	unsigned int tamanioDireccion = sizeof(direccionCliente);

	int servidor = initServer(gameCardConfig->puertoGameCard,
			gameCardConfig->puertoGameCard);

	log_info(logger, "ESCUCHANDO CONEXIONES");
	log_info(logger, "iiiiIIIII!!!");

	while (1) {

		socketDelCliente[contadorConexiones] = accept(servidor,
				(void*) &direccionCliente, &tamanioDireccion);

		if (socketDelCliente >= 0) {

			//log_info(logEntrega, "Se ha aceptado una conexion: %i\n",
			//socketDelCliente[contadorConexiones]);
			if ((pthread_create(&threadId[contadorConexiones], NULL,
					recvMensajesGameCard,
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

void* consumirMensajesGameCard() {

	while (1) {
		pthread_t hilito;
		sem_wait(&bandejaCounter);
		//pthread_mutex_lock(&bandejaMensajes_mutex);
		pthread_create(&hilito, NULL, procesarMensajeGameCard, NULL);
		//pthread_mutex_unlock(&bandejaMensajes_mutex);

	}

	return NULL;
}



void* suscribirseNewPokemon() {

	int socketBroker = crearConexion(gameCardConfig->ipBroker,
			gameCardConfig->puertoBroker, gameCardConfig->tiempoReintentoConexion);

	pthread_mutex_t mutexRecibir; //este semaforo no lo entiendo muy bien, pero funciona, sin él se rompe todo.
	pthread_mutex_init(&mutexRecibir, NULL);
	suscribirseNew(gameCardConfig->nombreProceso, 0, socketBroker);

	t_paquete *bufferLoco;
	bufferLoco = malloc(sizeof(t_paquete));
	int flag = 1;
	printf("Esperando por un nuevo mensaje AppearedPokemon...\n");

	while (flag) {

		pthread_mutex_lock(&mutexRecibir);
		bufferLoco = recibirMensaje(socketBroker);

		if (bufferLoco != NULL) {
			queue_push(bandejaDeMensajesGameCard, bufferLoco); //falta mutex y contador
			pthread_mutex_unlock(&mutexRecibir);
			sem_post(&bandejaCounter);

		} else {

			liberarConexion(socketBroker);
									socketBroker = 0;
									while (socketBroker <= 0) {
										pthread_mutex_unlock(&mutexRecibir);
										sleep(gameCardConfig->tiempoReintentoConexion);

										socketBroker = crearConexionSinReintento(
												gameCardConfig->ipBroker, gameCardConfig->puertoBroker);
									}

									suscribirseNew(gameCardConfig->nombreProceso, 0, socketBroker);
		}
	}
	return NULL;
}
void* suscribirseGetPokemon() {

	int socketBroker =crearConexion(gameCardConfig->ipBroker,
			gameCardConfig->puertoBroker, gameCardConfig->tiempoReintentoConexion);

	pthread_mutex_t mutexRecibir; //este semaforo no lo entiendo muy bien, pero funciona, sin él se rompe todo.
	pthread_mutex_init(&mutexRecibir, NULL);
	suscribirseGet(gameCardConfig->nombreProceso, 0, socketBroker);

	t_paquete *bufferLoco;
	bufferLoco = malloc(sizeof(t_paquete));
	int flag = 1;
	printf("Esperando por un nuevo mensaje LocalizedPokemon...\n");

	while (flag) {

		pthread_mutex_lock(&mutexRecibir);
		bufferLoco = recibirMensaje(socketBroker);

		if (bufferLoco != NULL) {
			queue_push(bandejaDeMensajesGameCard, bufferLoco); //falta mutex y contador
			pthread_mutex_unlock(&mutexRecibir);
			sem_post(&bandejaCounter);

		} else {
			liberarConexion(socketBroker);
						socketBroker = 0;
						while (socketBroker <= 0) {
							pthread_mutex_unlock(&mutexRecibir);
							sleep(gameCardConfig->tiempoReintentoConexion);

							socketBroker = crearConexionSinReintento(
									gameCardConfig->ipBroker, gameCardConfig->puertoBroker);
						}

						suscribirseGet(gameCardConfig->nombreProceso, 0, socketBroker);
		}
	}
	return NULL;
}
void* suscribirseCatchPokemon() {

	int socketBroker = crearConexion(gameCardConfig->ipBroker,
			gameCardConfig->puertoBroker, gameCardConfig->tiempoReintentoConexion);

	pthread_mutex_t mutexRecibir; //este semaforo no lo entiendo muy bien, pero funciona, sin él se rompe todo.
	pthread_mutex_init(&mutexRecibir, NULL);
	suscribirseCatch(gameCardConfig->nombreProceso, 0, socketBroker);

	t_paquete *bufferLoco;
	bufferLoco = malloc(sizeof(t_paquete));
	int flag = 1;
	printf("Esperando por un nuevo mensaje CatchPokemon...\n");

	while (flag) {

		pthread_mutex_lock(&mutexRecibir);
		bufferLoco = recibirMensaje(socketBroker);

		if (bufferLoco != NULL) {
			queue_push(bandejaDeMensajesGameCard, bufferLoco); //a esta bandeja le falta contador y mutex.
			pthread_mutex_unlock(&mutexRecibir);
			sem_post(&bandejaCounter);

		} else {

			liberarConexion(socketBroker);
									socketBroker = 0;
									while (socketBroker <= 0) {
										pthread_mutex_unlock(&mutexRecibir);
										sleep(gameCardConfig->tiempoReintentoConexion);

										socketBroker = crearConexionSinReintento(
												gameCardConfig->ipBroker, gameCardConfig->puertoBroker);
									}

									suscribirseCatch(gameCardConfig->nombreProceso, 0, socketBroker);
		}
	}
	return NULL;
}
void* suscribirseABroker() {

	pthread_t hilo1;
	pthread_t hilo2;
	pthread_t hilo3;


	pthread_create(&hilo1, NULL, (void*) suscribirseNewPokemon,NULL);
	pthread_create(&hilo2, NULL, (void*) suscribirseGetPokemon,NULL);
	pthread_create(&hilo3, NULL, (void*) suscribirseCatchPokemon, NULL);

	return NULL;
}




t_bitarray* crear_bitmap(){
	char* rutaBitmap = crearRutaArchivo(RUTA_BITMAP_GENERAL);

	int cantidadDeBloques = 5192;

	int fd = open(rutaBitmap, O_CREAT | O_RDWR, 0664);

	if (fd == -1) {
		perror("open file");
		exit(1);
	}

	ftruncate(fd, cantidadDeBloques);

	void* bmap = mmap(NULL, cantidadDeBloques, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (bmap == MAP_FAILED) {
		perror("mmap");
		close(fd);
		exit(1);
	}

	t_bitarray* bitmap = bitarray_create_with_mode((char*) bmap, cantidadDeBloques, LSB_FIRST);

	size_t tope = bitarray_get_max_bit(bitmap);

	for(int i = 0; i < tope; i++){

		 bitarray_clean_bit(bitmap, i);
	}
	msync(bmap,64,MS_SYNC);

	close(fd);

	return bitmap;
}

