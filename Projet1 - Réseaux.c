#include <stdio.h> // Pour printf()
#include <stdlib.h> // Pour exit()
#include <string.h> // Pour memset()
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> // pour memset
#include <netinet/in.h> // pour struct sockaddr_in
#include <arpa/inet.h> // pour htons et inet_aton
#include <unistd.h> // pour sleep
#include <poll.h> // pour poll()

#define PORT IPPORT_USERRESERVED // = 5000
#define LG_MESSAGE 256
#define LG_LOGIN 50
#define MAX_USERS 10


void motClient();

/*typedef struct pollfd
{
	int fd; // file descriptor
	short event; // requested events
	short revents // returned events
}pollfd;*/

typedef struct User
{
	int socketclient;
	char login[LG_LOGIN];
}User;
 
int main(int argc, char const *argv[])
{
	int socketEcoute;
	int /*ecrits,*/ lus; // nb d’octets ecrits et lus
	//int retour;
	int socketDialogue;
	struct sockaddr_in pointDeRencontreLocal;
	socklen_t longueurAdresse;
	struct sockaddr_in pointDeRencontreDistant;
	struct pollfd pollfds[MAX_USERS + 1];
	//char messageEnvoi[LG_MESSAGE]; // le message de la couche Application !
	char messageRecu[LG_MESSAGE]; // le message de la couche Application !
	User users[MAX_USERS];
	

	memset(users, '\0', MAX_USERS*sizeof(User));

	// Crée un socket de communication
	socketEcoute = socket(PF_INET, SOCK_STREAM, 0);
	// 0 indique que l’on utilisera le protocole par défaut associé à SOCK_STREAM soit TCP
	
	// Teste la valeur renvoyée par l’appel système
	socket(PF_INET, SOCK_STREAM, 0);

	if(socketEcoute < 0) // échec ?
	{
		perror("socket"); // Affiche le message d’erreur
		exit(-1); // On sort en indiquant un code erreur
	}

	printf("Socket créée avec succès ! (%d)\n", socketEcoute);

	// On prépare l’adresse d’attachement locale
	longueurAdresse = sizeof(struct sockaddr_in);
	memset(&pointDeRencontreLocal, 0x00, longueurAdresse);
	pointDeRencontreLocal.sin_family = PF_INET;
	pointDeRencontreLocal.sin_addr.s_addr = htonl(INADDR_ANY);
	// toutes les interfaces locales disponibles
	pointDeRencontreLocal.sin_port = htons(PORT);

	// On demande l’attachement local de la socket
	if((bind(socketEcoute, (struct sockaddr *)&pointDeRencontreLocal, longueurAdresse)) < 0)
	{
		perror("bind");
		exit(-2);
	}

	printf("Socket attachée avec succès !\n");

	// On fixe la taille de la file d’attente à 5 (pour les demandes de connexion non encore traitées)
	if(listen(socketEcoute, 5) < 0)
	{
		perror("listen");
		exit(-3);
	}
	
	printf("Socket placée en écoute passive ...\n");
	// boucle d’attente de connexion : en théorie, un serveur attend indéfiniment !
	
	while(1)
	{
		int nevents, i, j;
		int nfds = 0;

		// Liste des sockets à écouter
		pollfds[nfds].fd = socketEcoute;
		pollfds[nfds].events = POLLIN;
		pollfds[nfds].revents = 0;

		nfds++;

		// Remplissage du tableau avec les infos (pollfs) des utilisateurs co
		for (i = 0; i < MAX_USERS; i++)
		{
			if (users[i].socketclient != 0) // J'ai changé socket en socketclient
			{
				pollfds[nfds].fd = users[i].socketclient; // J'ai changé socket en socketclient
				pollfds[nfds].events = POLLIN;
				pollfds[nfds].revents = 0;

				nfds++;
			}
		}

		/* Structure Pollfd pour information uniquement
		struct pollfd {
			int fd; // File Descriptor
			short events; // Requested Events
			short revents; // Returned Events
		}; */

		// Demander à poll s'il a vu des évènements
		nevents = poll(pollfds, nfds, -1);

		// Si poll a vu des évènements
		if (nevents > 0)
		{
			for (i = 0; i < nfds; i++)
			{
				if (pollfds[i].revents != 0)
				{
					// S'il s'agit d'un évènement de la socket d'écoute (= nouvel utilisateur)
					if (pollfds[i].fd == socketEcoute)
					{
						socketDialogue = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);

						if (socketDialogue < 0)
						{
							perror("accept");
							exit(-4);
						}

						// Ajout de l'utilisateur
						for (j = 0; j < MAX_USERS; j++)
						{
							// S'il y a une place de libre dans le tableau des utilisateurs connectés, on ajoute le nouvel utilisateur au tableau
							if (users[j].socketclient == 0) // J'ai changé socket en socketclient
							{
								users[j].socketclient = socketDialogue; // J'ai changé socket en socketclient

								snprintf(users[j].login, LG_LOGIN, "anonymous%d", socketDialogue);
								printf("Ajout de l'utilisateur %s en position %d\n", users[j].login, j);
								
								break;
							}

							// Si aucune place n'a été trouvée
							if (j == MAX_USERS)
							{
								printf("Plus de place de disponible pour ce nouvel utilisateur\n");
								close(socketDialogue);
							}
						}
					}
					
					// Sinon, il s'agit d'un évènement d'un utilisateur (message, commande, etc)
					else
					{
						// On cherche quel utilisateur a fait la demande grâce à sa socket
						for (j = 0; j < MAX_USERS; j++)
						{
							if (users[j].socketclient == pollfds[i].fd) // J'ai changé socket en socketclient
							{
								break;
							}
						}

						// Si aucun utilisateur n'a été trouvé
						if (j == MAX_USERS)
						{
							printf("Utilisateur inconnu\n");
							break;
						}
						
						// On réceptionne les données du client
						lus = read(pollfds[i].fd, messageRecu, LG_MESSAGE*sizeof(char));

						switch (lus)
						{
							case -1 : /* Une erreur */
								perror("read");
								exit(-5);

							case 0 : /* La socket est fermée */
								printf("L'utilisateur %s en position %d a quitté le tchat\n", users[j].login, j);
								memset(&users[j], '\0', sizeof(struct User));

							default: /* Réception de n octets */
								printf("Message reçu de %s : %s (%d octets)\n\n", users[j].login, messageRecu, lus);
								motClient(pollfds[i].fd,messageRecu);
								//dissect protocole msg(users, pollfds[i].fd, messageRecu);
								//memset(messageRecu, '\0', LG_MESSAGE*sizeof(char));
						}
					}
				}
			}
			
		}

		else
		{
			printf("poll() a renvoyé %d\n", nevents);
		}
	}

	// On ferme la ressource avant de quitter
	close(socketEcoute);
	
	return 0;
}

void init(char *commande) {
	memset(commande,0,50);
}

void motClient(int socketclient, char*commande)
{
	if (strncmp(commande,"!login",6)==0)
	{
		
	}
	else if (strncmp(commande,"!help",5)==0)
	{
		dprintf(socketclient,"Liste des fonctions :\n");
		dprintf(socketclient,"  -  !login   ->   modifier son login\n");
		dprintf(socketclient,"  -  !help    ->   liste des actions possibles\n");
		dprintf(socketclient,"  -  !ping    ->   pinguer le serveur\n");
		dprintf(socketclient,"  -  !list    ->   liste les logins\n");
		dprintf(socketclient,"  -  !msg     ->   envoyer un message\n");
		dprintf(socketclient,"  -  !serveur ->   affiche l'IP du serveur\n");
		dprintf(socketclient,"  -  !port    ->   affiche le port du serveur\n");
		dprintf(socketclient,"  -  !exit    ->   déconnexion\n");
	}
	else if (strncmp(commande,"!list",5)==0)
	{
		printf("liste\n");
	}
	else if (strncmp(commande,"!msg",4)==0)
	{
		//char message[LG_MESSAGE];
		//sendmsg(socketDialogue, message,MSG_OOB);
	}
	else{
		exit(5);
	}
}
