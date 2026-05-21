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
			cnt ++;
			printf("Compteur : %d\n", cnt);
			sleep(2);
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
				printf("Vous avez écrit : %s\n", buffer);
			}
		}
	}
	if(pid != 0) { // thread parent
		/* Termine le thread enfant */
		kill(pid, SIGTERM);
	}

	return 0;
}
