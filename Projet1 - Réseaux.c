#include <stdio.h>
#include <stdlib.h> // pour exit
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> // pour memset
#include <netinet/in.h> // pour struct sockaddr_in
#include <arpa/inet.h> // pour htons et inet_aton
#include <unistd.h> // pour sleep

#define PORT IPPORT_USERRESERVED // = 5000
#define LG_MESSAGE 256
#define LG_LOGIN 50

typedef struct pollfd
{
	int fd; // file descriptor
	short event; // requested events
	short revents // returned events
}pollfd;

typedef struct User
{
	int socketclient;
	char login[LG_LOGIN];
}User;
 
int main(int argc, char const *argv[])
{
	int socketEcoute;
	struct sockaddr_in pointDeRencontreLocal;

	socklen_t longueurAdresse;
	
	int socketDialogue;
	struct sockaddr_in pointDeRencontreDistant;
	char messageEnvoi[LG_MESSAGE]; // le message de la couche Application !
	char messageRecu[LG_MESSAGE]; // le message de la couche Application !
	
	int ecrits, lus; // nb d’octets ecrits et lus
	int retour;

	// Crée un socket de communication
	socketEcoute = socket(PF_INET, SOCK_STREAM, 0);
	// 0 indique que l’on utilisera le protocole par défaut associé à SOCK_STREAM soit TCP
	
	// Teste la valeur renvoyée par l’appel système
	socket();

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
		memset(messageEnvoi, 0x00, LG_MESSAGE*sizeof(char));
		memset(messageRecu, 0x00, LG_MESSAGE*sizeof(char));

		printf("Attente d’une demande de connexion (quitter avec Ctrl-C)\n\n");

		// c’est un appel bloquant
		socketDialogue = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, & longueurAdresse);

		if (socketDialogue < 0)
		{
			perror("accept");
			close(socketDialogue);
			close(socketEcoute);
			exit(-4);
		}

		// On réceptionne les données du client
		lus = read(socketDialogue, messageRecu, LG_MESSAGE*sizeof(char));
		// ici appel bloquant

		switch(lus)
		{
			case -1 : // une erreur !
				perror("read");
				close(socketDialogue);
				exit(-5);
			case 0 : // la socket est fermée
				fprintf(stderr, "La socket a été fermée par le client !\n\n");
				close(socketDialogue);
				return 0;
			default: // réception de n octets
				printf("Message reçu : %s (%d octets)\n\n", messageRecu, lus);
		}

		// On envoie des données vers le client
		sprintf(messageEnvoi, "ok\n");
		ecrits = write(socketDialogue, messageEnvoi, strlen(messageEnvoi));
		
		switch(ecrits)
		{
			case -1 : // une erreur !
				perror("write");
				close(socketDialogue)
				exit(-6);
			case 0 : // la socket est fermée
				fprintf(stderr, "La socket a été fermée par le client !\n\n");
				close(socketDialogue);
			return 0;
			default: /* envoi de n octets */
				printf("Message %s envoyé (%d octets)\n\n", messageEnvoi, ecrits);
		}

		// On ferme la socket de dialogue et on se replace en attente...
		close(socketDialogue);
	}

	// On ferme la ressource avant de quitter
	close(socketEcoute);
	return 0;
}

void motClient(User)
{
	if (mot == !login) // à finir
	{
		printf("Ancien login : %s\n", );
		printf("Nouveau login : ");
		scanf("%s%*[&]\n\n", &);

		printf("Bonjour %s\n", );

		systeme("clear");
	}

	else if (mot == !help)
	{
		printf("Liste des fonctions :\n");
		printf("  -  !login   ->   modifier son login\n");
		printf("  -  !help    ->   liste des actions possibles\n");
		printf("  -  !ping    ->   pinguer le serveur\n");
		printf("  -  !list    ->   liste les logins\n");
		printf("  -  !msg     ->   envoyer un message\n");
		printf("  -  !serveur ->   affiche l'IP du serveur\n");
		printf("  -  !port    ->   affiche le port du serveur\n");
		printf("  -  !exit    ->   déconnexion\n");
	}

	else if (mot == !list)
	{
		printf("%s\n", );
	}

	else if (mot == !msg)
	{
		char message[50];

		printf("Entrez votre message :\n");
		scanf("%s", &message);

		// envoi du message à finir

	}

	else if (mot == !ping)
	{
		
	}

	else
		exit();
}