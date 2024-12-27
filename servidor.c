#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define PORTA "2002"
#define MAXMSG 256
#define ENVIAR '0'
#define ATUALIZAR '1'
#define NUMERO_CLIENTES 10
#define NUMERO_MENSAGENS 10

/* msg:
0: emissor
1: env/rcb
2: código destinatario
3 em diante: mensagem */

int main(int argc, char**argv) {
	struct addrinfo hints, *res, *p;
	struct sockaddr_storage cliente_addr;
	socklen_t sin_size;
	int socket_fd, novo_fd, cliente, numero_mensagens_recebidas[NUMERO_CLIENTES] = {};
	char msg[MAXMSG], msg_armazenada[NUMERO_CLIENTES][NUMERO_MENSAGENS][MAXMSG];

	memset(&hints, 0, sizeof hints);
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	if(getaddrinfo(0, PORTA, &hints, &res) != 0) {
		puts("Erro no getaddrinfo!");
		return 1;
	}

	for(p = res; p != NULL; p = p->ai_next) {
		socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		
		if(socket_fd == -1) continue;
		
		if(bind(socket_fd, res->ai_addr, res->ai_addrlen) == 0) break;

		close(socket_fd);
	}	

	freeaddrinfo(res);

	if (p == NULL) { // Nenhum conseguiu realizar o bind 
		puts("Erro ao realizar bind!");
		return 1;
	}
	
	if (listen(socket_fd, 5) == -1) {
		puts("Erro no listen!");
		return 1;
	}

	while(1){
		memset(msg, 0, sizeof msg);
		sin_size = sizeof cliente_addr;
		novo_fd = accept(socket_fd, (struct sockaddr *)&cliente_addr, &sin_size);
		if(novo_fd == -1) {
			continue;
		}
		if (recv(novo_fd, msg, MAXMSG, 0) == -1) {
			puts("Erro ao receber mensagem");
			return 1;
		}
		printf("Recebido: %s\n", msg);
		int emissor = msg[0] - 48, env_rcb = msg[1], destinatario = msg[2] - 48;
        
		if (env_rcb == ENVIAR && numero_mensagens_recebidas[destinatario] < 100) {
			int n = numero_mensagens_recebidas[destinatario]++;
			sprintf(msg_armazenada[destinatario][n], "Recebido de %d: %s\n", emissor, msg+3);
		}
		else {
			char msg_final[MAXMSG]={};
			for (int n = 0;n < numero_mensagens_recebidas[emissor];n++) { 
				strcat(msg_final,msg_armazenada[emissor][n]); 
			}
			memset(&numero_mensagens_recebidas[emissor],0,sizeof &numero_mensagens_recebidas[emissor]);
			numero_mensagens_recebidas[emissor] = 0;
			send(novo_fd,msg_final, MAXMSG, 0);
			close(novo_fd);
		}	

	}
	return 0;
}
