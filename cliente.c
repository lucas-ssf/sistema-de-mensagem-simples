#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define PORTA "2002"
#define ENDERECO "127.0.0.1"
#define MAXMSG 256
#define CLIENTE '0'
#define ENVIAR '0'
#define ATUALIZAR '1'

/* argv:
1: enviar/atualizar
2: código do destinatário 
3: mensagem (em caso de enviar) */

/* msg:
0: cliente
1: enviar/atualizar
2: código destinatário
3 em diante: mensagem */

int main(int argc, char**argv) {
	struct addrinfo hints, *res, *p;
	int socket_fd;
	char env_rcb = *argv[1], destinatario = *argv[2];
	char msg[MAXMSG]={CLIENTE, env_rcb, destinatario};
	FILE *mensagens_recebidas = fopen("./recebidas.txt", "a");

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	if(getaddrinfo(ENDERECO, PORTA, &hints, &res) != 0) {
		puts("Erro no getaddrinfo!");
		return 1;
	}

	for(p = res; p != NULL; p = p->ai_next) {
		socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		
		if(socket_fd == -1) continue;
		
		if(connect(socket_fd, res->ai_addr, res->ai_addrlen) != -1) break;

		close(socket_fd);
	}	

	freeaddrinfo(res);

	if (p == NULL) { // Nenhum endereço foi conectado
		puts("Erro ao realizar conexão!");
		return 1;
	}

	// Duas opções: enviar ou atualizar
	if (env_rcb - 48 == 0) { //enviar
		if (argv[3] == NULL) msg[3] = 0;
		else {
			strncpy(msg+3, argv[3], MAXMSG-3);
			msg[MAXMSG] = 0;
		}
		if (send(socket_fd, msg, MAXMSG, 0) == -1) {
			puts("Erro ao enviar mensagem");
			return 1;
		}
		printf("Enviado: %s\n", msg);
	}
	else { //atualizar
		if (send(socket_fd, msg, MAXMSG, 0) == -1) {
			puts("Erro ao solicitar atualização!");
			return 1;
		}
		if (recv(socket_fd, msg, MAXMSG, 0) == -1) {
			puts("Erro ao receber mensagem");
			return 1;
		}
		fputs(msg, mensagens_recebidas);
	}
	return 0;
}
