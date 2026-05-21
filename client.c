#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define MSG_LEN 100         // taille max des messages autorisés
#define IN_FILT "%[^\n]%*c" // filtre le prompt : récupère tout jusqu'au premier retour à la ligne
#define CMD_QUIT "/q"       // commande du prompt pour quitter

int main(int argc , char *argv[]) {

	int sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(sd < 0){
		perror("socket");
		return 1;
	}

	printf("[CONSOLE] Socket crée\n");

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(4242);
	addr.sin_addr.s_addr = inet_addr("169.254.61.46");


	int connect_err = connect(sd, (struct sockaddr*)&addr, sizeof(addr));

    if(connect_err < 0){
        perror("connect");
        return 1;
    }

	printf("[CONSOLE] Socket connecté! \n");


	/* Ce buffer recevra les messages à transmettre */
	char buffer[MSG_LEN];

	int cnt = 0;

	/* Fork: création d'un thread enfant */
	int pid = fork();
	/* Flag pour arrêter le programme */
	int quit = 0;
	while(!quit) {
		if(pid==0) { // thread enfant
			/* Fait quelque chose */



			ssize_t lrecv = recv(sd, &buffer, 32, 0);

			if(lrecv == 0){
				printf("Client deconnecté\n");
			} else if(lrecv < 0){
				perror("recv");
			} else {

				buffer[lrecv] = '\0';
				printf("Message reçu: %s\n", buffer);

			}


		} else { // thread parent
			/* Lie le contenu du prompt */
			scanf(IN_FILT, buffer);
			/* Gestion des commandes du prompt */
			if(strcmp(buffer, CMD_QUIT) == 0) {
				/* Commande pour quitter */
				quit = 1;
				printf("Au revoir !\n");
			} else {
				/* Pas une commande -> on affiche le contenu du prompt */
				if(buffer != 0){
					ssize_t send_err = send(sd, &buffer, sizeof(buffer), 0);

					if(send_err < 0){
						perror("send");
						return 1;
					}

					for(int i = 0; i < MSG_LEN; i++){
						buffer[i] = "";

					}
				}
				

			}
		}
	}
	if(pid != 0) { // thread parent
		/* Termine le thread enfant */
		kill(pid, SIGTERM);
	}

	return 0;
}
