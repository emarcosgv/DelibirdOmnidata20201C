#include "broker.h"

int main(void) {
	inicializarLogger();
	inicializarColasBroker();
	cargarConfigBROKER();

	char *ip=brokerConf->ipBroker;
	char *puerto=brokerConf->puertoBroker;
	iniciarServidor(ip,puerto);


	return EXIT_SUCCESS;
}
